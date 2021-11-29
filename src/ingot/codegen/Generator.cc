#include "Generator.hh"

#include <ingot/codegen/CodegenVisitor.hh>
#include <ingot/codegen/ListOperations.hh>

#include <ingot/semantics/SemanticTree.hh>

#include <ingot/Error.hh>

#include <llvm/IR/IRBuilder.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>


#include <iostream>

namespace ingot::codegen
{
    Generator::Generator()
    : m_context() {
        LLVMInitializeNativeTarget();
        LLVMInitializeNativeAsmParser();
        LLVMInitializeNativeAsmPrinter();
    }

    void
    Generator::run(const semantics::SemanticTree& semTree) {
        llvm::IRBuilder<> builder{m_context};

        auto i8  = builder.getInt8Ty();
        auto i32 = builder.getInt32Ty();
        auto i64 = builder.getInt64Ty();
        
        auto i8p = builder.getInt8PtrTy();

        auto exitSuccess = llvm::ConstantInt::get(i32, 0, true);

        auto mainModule = std::make_unique<llvm::Module>("ingot_main_module", m_context);

        TypeContext typeContext{builder, *mainModule};
        ListOperations::Collection listOperationsCollection{typeContext};

        // printf declaration
        llvm::FunctionType* printfPrototype = llvm::FunctionType::get(i8p, true);
        llvm::Function* printfFunc = llvm::Function::Create(printfPrototype, llvm::Function::ExternalLinkage, "printf", mainModule.get());

        // print_string declaration
        llvm::Type* stringLLVMType = typeContext.getLLVMType(ast::String::getType());
        llvm::PointerType* stringPtrLLVMType = llvm::PointerType::get(stringLLVMType, 0);
        ListOperations stringOperations = listOperationsCollection.get(ast::String::getType());
        llvm::FunctionType* printStringPrototype = llvm::FunctionType::get(builder.getVoidTy(), {stringPtrLLVMType}, false);
        llvm::Function* printStringFunc = llvm::Function::Create(printStringPrototype, llvm::Function::InternalLinkage, "ingot_print_string", mainModule.get());

        // User declarations
        llvm::Function* userMain = nullptr;
        ast::Type userMainRetType;
        std::map<const ast::FunctionDefinition*, llvm::Function*> userFunctions;
        for (const ast::FunctionDefinition& definition : semTree) {
            const ast::FunctionType& funcType = definition.getFunction().getFunctionType();
            const ast::Type& retType = funcType.getReturnType();
            llvm::Type* llvmRetType = typeContext.getLLVMType(retType);
            std::vector<llvm::Type*> argumentTypes;
            for (const ast::Type& astType : funcType.getArgumentTypes()) {
                argumentTypes.push_back(typeContext.getLLVMType(astType));
            }
            llvm::FunctionType* prototype = llvm::FunctionType::get(llvmRetType, argumentTypes, false); // TODO: Handle arguments
            std::string userName = definition.getName();
            
            std::string name = userFunctionPrefix + userName;
            llvm::Function* function = llvm::Function::Create(prototype, llvm::Function::InternalLinkage, name, mainModule.get());
            userFunctions[&definition] = function;
            if (userName == "main") {
                userMain = function;
                userMainRetType = retType;
            }
        }

        // print string definition
        {
            llvm::BasicBlock* entryBlock = llvm::BasicBlock::Create(m_context, "entry", printStringFunc);
            llvm::BasicBlock* emptyStringBlock = llvm::BasicBlock::Create(m_context, "empty_string", printStringFunc);
            llvm::BasicBlock* printChunkBlock = llvm::BasicBlock::Create(m_context, "print_chunk", printStringFunc);
            llvm::BasicBlock* recurseBlock = llvm::BasicBlock::Create(m_context, "recurse", printStringFunc);
            builder.SetInsertPoint(entryBlock);
            llvm::Argument* inputPtr = printStringFunc->getArg(0);
            
            llvm::Value* inputCountPtr = builder.CreateStructGEP(inputPtr, 2, "inputCountPtr");
            llvm::Value* inputCount = builder.CreateLoad(inputCountPtr);
            llvm::IntegerType* counterType = typeContext.getLLVMTypeForListCounterMember();
            llvm::ConstantInt* zeroCountConst = llvm::ConstantInt::get(counterType, 0);
            llvm::Value* isEmpty = builder.CreateICmpEQ(inputCount, zeroCountConst, "isEmpty");
            builder.CreateCondBr(isEmpty, emptyStringBlock, printChunkBlock);

            builder.SetInsertPoint(emptyStringBlock);
            builder.CreateRetVoid();

            builder.SetInsertPoint(printChunkBlock);
            llvm::ArrayType* arrayType = typeContext.getLLVMTypeForListArrayMember(ast::String::getType());
            llvm::Value* arrayLength = llvm::ConstantInt::get(counterType, arrayType->getArrayNumElements());
            llvm::Value* index = builder.CreateSub(arrayLength, inputCount, "inputIndex");
            llvm::Value* inputArrayPtr = builder.CreateStructGEP(inputPtr, 1, "inputArrayPtr");
            llvm::Value* firstElementPtr = builder.CreateInBoundsGEP(inputArrayPtr, {zeroCountConst, index}, "firstElementPtr");
            llvm::Constant* formatString = builder.CreateGlobalStringPtr("%.*s");
            builder.CreateCall(printfFunc, {formatString, inputCount, firstElementPtr}, "printf_of_chunk");
            llvm::Value* nextPtr = builder.CreateStructGEP(inputPtr, 0, "nextPtr");
            llvm::Value* next = builder.CreateLoad(nextPtr);
            llvm::Value* nextIsNull = builder.CreateIsNull(next, "nextIsNull");
            builder.CreateCondBr(nextIsNull, emptyStringBlock, recurseBlock);

            builder.SetInsertPoint(recurseBlock);
            builder.CreateCall(printStringFunc, {next});
            builder.CreateRetVoid();
        }

        // User definitions
        for (const auto&[definitionPtr, function] : userFunctions) {
            llvm::BasicBlock* body = llvm::BasicBlock::Create(m_context, "entry", function);
            builder.SetInsertPoint(body);
            CodegenVisitor visitor{function, typeContext, listOperationsCollection, semTree, userFunctions};
            CodegenVisitorInfo info = definitionPtr->getFunction().getExpression().traverse(visitor);
            builder.CreateRet(info.m_value);
        }

        if (userMain) {
            // Main declaration
            llvm::FunctionType* mainPrototype = llvm::FunctionType::get(i32, false);
            llvm::Function* mainFunc = llvm::Function::Create(mainPrototype, llvm::Function::ExternalLinkage, "main", mainModule.get());

            // Main defintion
            llvm::BasicBlock *mainBody = llvm::BasicBlock::Create(m_context, "entry", mainFunc);
            builder.SetInsertPoint(mainBody);
            if (userMainRetType.getVariant() == ast::Type::Variant::Integer) {
                std::string formatStr = "%lli";
                llvm::Constant* formatStrConstant = builder.CreateGlobalStringPtr(formatStr);
                llvm::CallInst* userMainCall = builder.CreateCall(userMain);
                builder.CreateCall(printfFunc, {formatStrConstant, userMainCall});
                builder.CreateRet(exitSuccess);
            } else if (userMainRetType == ast::String::getType()) {
                llvm::CallInst* userMainCall = builder.CreateCall(userMain);
                llvm::AllocaInst* stringPtr = builder.CreateAlloca(typeContext.getLLVMType(ast::String::getType()), nullptr, "stringPtr");
                builder.CreateStore(userMainCall, stringPtr);
                builder.CreateCall(printStringFunc, stringPtr);
                builder.CreateRet(exitSuccess);
            } else {
                throw internal_error("unhandled type: " + userMainRetType.getName());
            }
        }
        

        {
            // Print and execute for now... need to figure out what to do with result.
            mainModule->print(llvm::outs(), nullptr);
            
            // std::cout << "++++++++++++++++++++++" << std::endl;
            // if (userMain) {
            //     llvm::ExecutionEngine *executionEngine = llvm::EngineBuilder(std::move(mainModule)).setEngineKind(llvm::EngineKind::JIT).create();
            //     llvm::Function *main = executionEngine->FindFunctionNamed(llvm::StringRef("main"));
            //     auto result = executionEngine->runFunction(main, {});
            // }
            
        }
        
    }
} // namespace ingot::codegen
