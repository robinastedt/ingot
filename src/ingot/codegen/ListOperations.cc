#include "ListOperations.hh"

#include <ingot/codegen/InsertPointGuard.hh>

#include <ingot/Error.hh>

#include <iostream>

namespace ingot::codegen
{

    namespace
    {

        /*
            struct List {
                List* next;
                Element element[blocksize];
                char count;
            };
        */
       constexpr unsigned int LIST_NEXT_IDX = 0,
                              LIST_ELEMENT_ARRAY_IDX = 1,
                              LIST_COUNT_IDX = 2;

        llvm::Function*
        generateEmptyFunction(TypeContext& typeContext, const ast::Type& type) {
            llvm::IRBuilder<>& builder = typeContext.getBuilder();
            llvm::Type* structType = typeContext.getLLVMType(type);
            llvm::FunctionType* funcType = llvm::FunctionType::get(structType, false);
            std::string twine = "ingot_" + type.getNameEncoded() + "_empty";
            llvm::Function* func = llvm::Function::Create(funcType, llvm::Function::InternalLinkage, twine, typeContext.getModule());
            llvm::BasicBlock *body = llvm::BasicBlock::Create(typeContext.getLLVMContext(), "entry", func);
            builder.SetInsertPoint(body);
            builder.CreateRet(llvm::ConstantAggregateZero::get(structType));
            return func;
        }

        llvm::Function*
        generateIsEmptyFunction(TypeContext& typeContext, const ast::Type& type) {
            llvm::IRBuilder<>& builder = typeContext.getBuilder();
            llvm::Type* structType = typeContext.getLLVMType(type);
            llvm::PointerType* pointerType = llvm::PointerType::get(structType, 0);
            llvm::IntegerType* i1 = llvm::IntegerType::get(typeContext.getLLVMContext(), 1);
            llvm::IntegerType* counterType = typeContext.getLLVMTypeForListCounterMember();

            llvm::FunctionType* funcType = llvm::FunctionType::get(i1, {pointerType}, false);
            std::string twine = "ingot_" + type.getNameEncoded() + "_is_empty";
            llvm::Function* func = llvm::Function::Create(funcType, llvm::Function::InternalLinkage, twine, typeContext.getModule());

            llvm::Argument* inputStructPtr = func->getArg(0);
            llvm::ConstantInt* counterZeroConstant = llvm::ConstantInt::get(counterType, 0, "counterConstOne");

            llvm::BasicBlock *entryBlock = llvm::BasicBlock::Create(typeContext.getLLVMContext(), "entry", func);
            builder.SetInsertPoint(entryBlock);
            llvm::Value* inputCountPtr = builder.CreateStructGEP(inputStructPtr, LIST_COUNT_IDX, "inputCountPtr");
            llvm::Value* inputCount = builder.CreateLoad(inputCountPtr, "inputCount");
            llvm::Value* isZero = builder.CreateICmpEQ(inputCount, counterZeroConstant, "isZero");
            builder.CreateRet(isZero);

            return func;
        }

        llvm::Function*
        generateAppendFunction(TypeContext& typeContext, const ast::Type& type) {
            llvm::IRBuilder<>& builder = typeContext.getBuilder();
            llvm::Type* structType = typeContext.getLLVMType(type);
            llvm::PointerType* pointerType = llvm::PointerType::get(structType, 0);
            llvm::Type* elementType = typeContext.getLLVMType(type.getSubtype());
            llvm::FunctionType* funcType = llvm::FunctionType::get(structType, {elementType, pointerType}, false);
            std::string twine = "ingot_" + type.getNameEncoded() + "_append";
            llvm::Function* func = llvm::Function::Create(funcType, llvm::Function::InternalLinkage, twine, typeContext.getModule());

            llvm::IntegerType* i64 = llvm::IntegerType::get(typeContext.getLLVMContext(), 64);

            llvm::Argument* inputElement = func->getArg(0);
            llvm::Argument* inputStructPtr = func->getArg(1);
            llvm::ArrayType* arrayType = typeContext.getLLVMTypeForListArrayMember(type.getSubtype());
            llvm::IntegerType* counterType = typeContext.getLLVMTypeForListCounterMember();
            llvm::ConstantInt* arrayLength = llvm::ConstantInt::get(counterType, arrayType->getArrayNumElements(), "arrayLength");
            llvm::Constant* structSize = llvm::ConstantExpr::getSizeOf(structType);
            llvm::ConstantInt* outputIndexOffset = llvm::ConstantInt::get(counterType, arrayType->getArrayNumElements() - 1, "outputIndexOffset");
            llvm::ConstantInt* counterOneConstant = llvm::ConstantInt::get(counterType, 1, "counterConstOne");
            llvm::ConstantInt* counterZeroConstant = llvm::ConstantInt::get(counterType, 0, "counterConstOne");
            
            llvm::BasicBlock *entryBlock = llvm::BasicBlock::Create(typeContext.getLLVMContext(), "entry", func);
            llvm::BasicBlock *notFullBlock = llvm::BasicBlock::Create(typeContext.getLLVMContext(), "not_full", func);
            llvm::BasicBlock *isFullBlock = llvm::BasicBlock::Create(typeContext.getLLVMContext(), "is_full", func);

            // Entry block
            llvm::Value* inputCount; // Output to null_full
            {
                builder.SetInsertPoint(entryBlock);
                llvm::Value* inputCountPtr = builder.CreateStructGEP(inputStructPtr, LIST_COUNT_IDX, "inputCountPtr");
                inputCount = builder.CreateLoad(inputCountPtr, "inputCount");
                
                llvm::Value* isFullCmp = builder.CreateICmpEQ(inputCount, arrayLength, "isFull");
                builder.CreateCondBr(isFullCmp, isFullBlock, notFullBlock);
            }

            // TODO: Clean up, a lot of duplicated code between the two following blocks
            // Stack allocation, GEP calculations etc can probably be moved into entry

            // Not full block
            {
                builder.SetInsertPoint(notFullBlock);

                // alloc output
                // *output = *input
                llvm::AllocaInst* outputStructPtr = builder.CreateAlloca(structType, nullptr, "outputPtr");
                builder.CreateStore(builder.CreateLoad(inputStructPtr, "loadInput"), outputStructPtr);
                //llvm::PHINode* notFullBlockPhiInputCount = builder.CreatePHI(counterType, 1, "inputCount");
                //notFullBlockPhiInputCount->addIncoming(inputCount, entryBlock); 

                // index_prev = blocksize - input->count 
                // index_current = index_prev_value - 1
                // index_current = (blocksize - 1) - input->count
                llvm::Value* outputIndex = builder.CreateSub(outputIndexOffset, inputCount /*can we use it directly like this?*/, "outputIndex");

                // output->elements[index_current] = elementToBeAppended;
                llvm::Value* outputArrayPtr = builder.CreateStructGEP(outputStructPtr, LIST_ELEMENT_ARRAY_IDX, "outputArrayPtr");
                llvm::Value* outputElementPtr = builder.CreateInBoundsGEP(outputArrayPtr, {counterZeroConstant, outputIndex}, "outputElementPtr");
                builder.CreateStore(inputElement, outputElementPtr);
                // output->count += 1
                llvm::Value* outputCount = builder.CreateAdd(inputCount, llvm::ConstantInt::get(counterType, 1), "inputCountPlusOne");
                llvm::Value* outputCountPtr = builder.CreateStructGEP(outputStructPtr, LIST_COUNT_IDX, "outputCountPtr");
                builder.CreateStore(outputCount, outputCountPtr);

                // return *output
                builder.CreateRet(builder.CreateLoad(outputStructPtr, "loadOutput"));
            }
            

            // Is full block
            {
                builder.SetInsertPoint(isFullBlock);
                llvm::AllocaInst* outputStructPtr = builder.CreateAlloca(structType, nullptr, "outputPtr");
                
                // next = new List()
                // *next = *input
                //TODO: account for reference counter
                llvm::Instruction* nextPtr =  builder.Insert(llvm::CallInst::CreateMalloc(isFullBlock, i64, structType, structSize, nullptr, nullptr), "nextPtr");
                builder.CreateStore(builder.CreateLoad(inputStructPtr, "loadInput"), nextPtr);
                
                // output->next = next
                llvm::Value* outputNextPtr = builder.CreateStructGEP(outputStructPtr, LIST_NEXT_IDX, "outputNextPtr");
                builder.CreateStore(nextPtr, outputNextPtr);

                // output->elements[blocksize - 1] = element
                llvm::Value* outputArrayPtr = builder.CreateStructGEP(outputStructPtr, LIST_ELEMENT_ARRAY_IDX, "outputArrayPtr");
                llvm::Value* outputElementPtr = builder.CreateInBoundsGEP(outputArrayPtr, {counterZeroConstant, outputIndexOffset}, "outputElementPtr");
                builder.CreateStore(inputElement, outputElementPtr);

                // output->count = 1
                llvm::Value* outputCountPtr = builder.CreateStructGEP(outputStructPtr, LIST_COUNT_IDX, "outputCountPtr");
                builder.CreateStore(counterOneConstant, outputCountPtr);

                // return *output
                builder.CreateRet(builder.CreateLoad(outputStructPtr, "loadOutput"));
            }
            return func;
        }

        llvm::Function*
        generateHeadFunction(TypeContext& typeContext, const ast::Type& type) {
            llvm::IRBuilder<>& builder = typeContext.getBuilder();
            llvm::Type* structType = typeContext.getLLVMType(type);
            llvm::PointerType* pointerType = llvm::PointerType::get(structType, 0);
            llvm::IntegerType* i1 = llvm::IntegerType::get(typeContext.getLLVMContext(), 1);
            llvm::IntegerType* counterType = typeContext.getLLVMTypeForListCounterMember();
            llvm::Type* elementType = typeContext.getLLVMType(type.getSubtype());
            llvm::ArrayType* arrayType = typeContext.getLLVMTypeForListArrayMember(type.getSubtype());

            llvm::ConstantInt* arrayLength = llvm::ConstantInt::get(counterType, arrayType->getArrayNumElements(), "arrayLength");

            llvm::FunctionType* funcType = llvm::FunctionType::get(elementType, {pointerType}, false);
            std::string twine = "ingot_" + type.getNameEncoded() + "_head";
            llvm::Function* func = llvm::Function::Create(funcType, llvm::Function::InternalLinkage, twine, typeContext.getModule());

            llvm::Argument* inputStructPtr = func->getArg(0);
            llvm::ConstantInt* counterZeroConstant = llvm::ConstantInt::get(counterType, 0, "counterConstOne");

            llvm::BasicBlock *entryBlock = llvm::BasicBlock::Create(typeContext.getLLVMContext(), "entry", func);
            builder.SetInsertPoint(entryBlock);
            llvm::Value* inputCountPtr = builder.CreateStructGEP(inputStructPtr, LIST_COUNT_IDX, "inputCountPtr");
            llvm::Value* inputCount = builder.CreateLoad(inputCountPtr, "inputCount");
            llvm::Value* inputArrayPtr = builder.CreateStructGEP(inputStructPtr, LIST_ELEMENT_ARRAY_IDX, "inputArrayPtr");
            llvm::Value* inputIndex = builder.CreateSub(arrayLength, inputCount, "inputIndex");
            llvm::Value* inputElementPtr = builder.CreateInBoundsGEP(inputArrayPtr, {counterZeroConstant, inputIndex}, "elementPtr");
            llvm::Value* headElement = builder.CreateLoad(inputElementPtr, "headElement");
            builder.CreateRet(headElement);

            return func;
        }

        llvm::Function*
        generateTailFunction(TypeContext& typeContext, const ast::Type& type) {
            llvm::IRBuilder<>& builder = typeContext.getBuilder();
            llvm::Type* structType = typeContext.getLLVMType(type);
            llvm::PointerType* pointerType = llvm::PointerType::get(structType, 0);
            llvm::IntegerType* i1 = llvm::IntegerType::get(typeContext.getLLVMContext(), 1);
            llvm::IntegerType* counterType = typeContext.getLLVMTypeForListCounterMember();
            llvm::Type* elementType = typeContext.getLLVMType(type.getSubtype());
            llvm::ArrayType* arrayType = typeContext.getLLVMTypeForListArrayMember(type.getSubtype());

            llvm::ConstantInt* arrayLength = llvm::ConstantInt::get(counterType, arrayType->getArrayNumElements(), "arrayLength");
            llvm::ConstantInt* counterOneConstant = llvm::ConstantInt::get(counterType, 1, "counterConstOne");

            llvm::FunctionType* funcType = llvm::FunctionType::get(structType, {pointerType}, false);
            std::string twine = "ingot_" + type.getNameEncoded() + "_tail";
            llvm::Function* func = llvm::Function::Create(funcType, llvm::Function::InternalLinkage, twine, typeContext.getModule());

            llvm::Argument* inputStructPtr = func->getArg(0);

            llvm::BasicBlock *entryBlock = llvm::BasicBlock::Create(typeContext.getLLVMContext(), "entry", func);
            llvm::BasicBlock *oneElementBlock = llvm::BasicBlock::Create(typeContext.getLLVMContext(), "one_element", func);
            llvm::BasicBlock *hasNextBlock = llvm::BasicBlock::Create(typeContext.getLLVMContext(), "has_next", func);
            llvm::BasicBlock *noNextBlock = llvm::BasicBlock::Create(typeContext.getLLVMContext(), "no_next", func);
            llvm::BasicBlock *multipleElementsBlock = llvm::BasicBlock::Create(typeContext.getLLVMContext(), "multiple_elements", func);
            
            // entry
            {
                builder.SetInsertPoint(entryBlock);
                llvm::Value* inputCountPtr = builder.CreateStructGEP(inputStructPtr, LIST_COUNT_IDX, "inputCountPtr");
                llvm::Value* inputCount = builder.CreateLoad(inputCountPtr, "inputCount");
                // inputCount == 0 is undefined behavior
                // Add code for debug build checking?
                llvm::Value* countIsOne = builder.CreateICmpEQ(inputCount, counterOneConstant, "countIsOne");
                builder.CreateCondBr(countIsOne, oneElementBlock, multipleElementsBlock);
            }

            // one_element
            {
                builder.SetInsertPoint(oneElementBlock);
                llvm::Value* inputNextPtr = builder.CreateStructGEP(inputStructPtr, LIST_NEXT_IDX, "inputNextPtr");
                llvm::Value* inputNext = builder.CreateLoad(inputNextPtr, "inputNext");
                llvm::Value* nextIsNull = builder.CreateIsNull(inputNext, "nextIsNull");
                builder.CreateCondBr(nextIsNull, noNextBlock, hasNextBlock);
            }

            // has_next
            {
                builder.SetInsertPoint(hasNextBlock);
                llvm::Value* inputNextPtr = builder.CreateStructGEP(inputStructPtr, LIST_NEXT_IDX, "inputNextPtr");
                llvm::Value* inputNext = builder.CreateLoad(inputNextPtr, "inputNext");
                llvm::Value* copy = builder.CreateLoad(inputNext, "copy");
                builder.CreateRet(copy);
            }

            // no_next
            {
                builder.SetInsertPoint(noNextBlock);
                builder.CreateRet(llvm::ConstantAggregateZero::get(structType));
            }

            // multiple_elements
            {
                builder.SetInsertPoint(multipleElementsBlock);
                llvm::AllocaInst* outputStructPtr = builder.CreateAlloca(structType, nullptr, "outputPtr");
                builder.CreateStore(builder.CreateLoad(inputStructPtr, "loadInput"), outputStructPtr);
                llvm::Value* outputCountPtr = builder.CreateStructGEP(outputStructPtr, LIST_COUNT_IDX, "outputCountPtr");
                llvm::Value* outputCount = builder.CreateLoad(outputCountPtr, "outputCount");
                llvm::Value* countMinusOne = builder.CreateSub(outputCount, counterOneConstant, "countMinusOne");
                builder.CreateStore(countMinusOne, outputCountPtr);
                llvm::Value* output = builder.CreateLoad(outputStructPtr, "tail");
                builder.CreateRet(output);
            }

            return func;
        }
    } // namespace
    

    ListOperations::ListOperations(TypeContext& typeContext, ast::Type type)
    : m_type(std::move(type))
    , m_subtype(m_type.getSubtype())
    , m_llvmType(typeContext.getLLVMType(m_type))
    , m_llvmFuncEmpty(generateEmptyFunction(typeContext, m_type))
    , m_llvmFuncIsEmpty(generateIsEmptyFunction(typeContext, m_type))
    , m_llvmFuncAppend(generateAppendFunction(typeContext, m_type))
    , m_llvmFuncHead(generateHeadFunction(typeContext, m_type))
    , m_llvmFuncTail(generateTailFunction(typeContext, m_type)) {}


    ListOperations::Collection::Collection(TypeContext& typeContext)
    : m_typeContext(typeContext) {}


    const ListOperations&
    ListOperations::Collection::get(const ast::Type& type) {
        if (type.getVariant() != ast::Type::Variant::List) {
            throw internal_error("Expected list type, got: " + type.getName());
        }
        auto it = m_listOperationsMap.find(type);
        if (it == m_listOperationsMap.end()) {
            InsertPointGuard _{m_typeContext.getBuilder()};
            auto insertPair = m_listOperationsMap.emplace(type, ListOperations{m_typeContext, type});
            assert(insertPair.second); // Insert took place
            return insertPair.first->second;
        }
        return it->second;
    }

    llvm::Function*
    ListOperations::getEmptyFunction() const {
        return m_llvmFuncEmpty;
    }

    llvm::Function*
    ListOperations::getIsEmptyFunction() const {
        return m_llvmFuncIsEmpty;
    }

    llvm::Function*
    ListOperations::getAppendFunction() const {
        return m_llvmFuncAppend;
    }

    llvm::Function*
    ListOperations::getHeadFunction() const {
        return m_llvmFuncHead;
    }

    llvm::Function*
    ListOperations::getTailFunction() const {
        return m_llvmFuncTail;
    }

} // namespace ingot::codegen
