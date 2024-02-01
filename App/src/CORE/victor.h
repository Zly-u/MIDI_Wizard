#pragma once

#include <iostream>
#include <cassert>
#include <cmath>

namespace zly {
    template<class victor_class>
    class victor_iterator{
    public:
        using ValueType     = typename victor_class::ValueType;
        using PointerType   = ValueType*;
        using ReferenceType = ValueType&;

    private:
        PointerType m_Ptr;

    public:
        explicit victor_iterator(PointerType ptr) : m_Ptr(ptr){};

        victor_iterator& operator++(){
            m_Ptr++;
            return *this;
        }

        victor_iterator operator++(int){
            victor_iterator iter = *this;
            ++(*this);
            return iter;
        }

        victor_iterator& operator--(){
            m_Ptr--;
            return *this;
        }

        victor_iterator operator--(int){
            victor_iterator iter = *this;
            --(*this);
            return iter;
        }

        ReferenceType operator[](int index){
            return *(m_Ptr + index);
        }

        PointerType operator->(){
            return m_Ptr;
        }

        ReferenceType operator*(){
            return *m_Ptr;
        }

        bool operator==(const victor_iterator& other){
            return m_Ptr == other.m_Ptr;
        }

        bool operator!=(const victor_iterator& other){
            return m_Ptr != other.m_Ptr;
        }
    };


    template<typename T>
    class victor {
    public:
        using ValueType     = T;
        using PointerType   = ValueType*;
        using ReferenceType = ValueType&;
        using Iterator      = victor_iterator<victor<ValueType>>;

    private:
        uint8_t m_AllocElementsAmount = 8;
        uint8_t m_deAllocElementsAmount = 16;

        PointerType m_ptr_start      = nullptr;
        PointerType m_ptr_end        = nullptr;
        PointerType m_ptr_storageEnd = nullptr;

        size_t m_size = 0;

    public:
        victor(){
//            std::cout << __PRETTY_FUNCTION__ << std::endl;
            const size_t toAlloc = sizeof(ValueType) * m_AllocElementsAmount;

            m_ptr_start      = (PointerType) calloc(m_AllocElementsAmount, sizeof(ValueType));
            m_ptr_end        = m_ptr_start;
            m_ptr_storageEnd = m_ptr_start + toAlloc + 1;

//            printf("[Construct] size: %d | full size: %d\n", size(), full_size());
        }

        victor(std::initializer_list<ValueType> init_list){
//            std::cout << __PRETTY_FUNCTION__ << std::endl;

            const size_t toAlloc = sizeof(ValueType) * calcBlocksToRealloc(init_list.size());

            m_ptr_start      = (PointerType) calloc(m_AllocElementsAmount, sizeof(ValueType));
            m_ptr_end        = m_ptr_start + init_list.size();
            m_ptr_storageEnd = m_ptr_start + toAlloc + 1;

            memcpy(m_ptr_start, init_list.begin(), toAlloc);
            recalcSize();
        }

        victor(victor&& other) noexcept{
//            std::cout << __PRETTY_FUNCTION__ << std::endl;

            if(&other == this){ return; }

            m_ptr_start = other.m_ptr_start;
            m_ptr_end   = other.m_ptr_end;

            other.m_ptr_start       = nullptr;
            other.m_ptr_end         = nullptr;
            other.m_ptr_storageEnd  = nullptr;
            other.m_size            = 0;
        }

        ~victor(){
//            std::cout << __PRETTY_FUNCTION__ << std::endl;
            delete[] m_ptr_start;
//            ::operator delete(m_ptr_start, sizeof(ValueType) * (m_ptr_storageEnd - m_ptr_start));
        }

        /* GENERAL FUNCTIONS */

        [[nodiscard]] size_t size() const{
            return m_size;
        }
        [[nodiscard]] size_t full_size() const{
            return (m_ptr_storageEnd - m_ptr_start) / sizeof(ValueType);
        }

        ValueType pop(){
            if(m_size == 0) { return; }

            ValueType temp = *m_ptr_end;
            delete m_ptr_end;

            m_ptr_end--;
            m_size--;

            return temp;
        }

        /* OPERATORS */

        ReferenceType operator[](const size_t index){
//            std::cout << __PRETTY_FUNCTION__ << std::endl;
            assert(m_ptr_start != nullptr);
            assert(m_ptr_storageEnd != nullptr);
            assert(index >= 0);


            // For when we go OOB of the available space
            if(index+1 > full_size()){
                const size_t elementsToAllocate = calcBlocksToRealloc(index);

//                printf("[realloc]\tindex: %d | ReAllocating: %d | prev Full Size: %d | ", index, elementsToAllocate, full_size());

                m_ptr_start      = (PointerType) realloc(m_ptr_start, sizeof(ValueType) * elementsToAllocate);
                m_ptr_end        = m_ptr_start + (index + 1);
                m_ptr_storageEnd = m_ptr_start + sizeof(ValueType) * elementsToAllocate + 1;
                recalcSize();

//                printf("new Full Size: %d\n", full_size());

                return *(m_ptr_start + index);
            }

            // For within the available space
            if(index+1 > size()){
//                printf("[resizing]\tindex: %d(%d) | Cur size: %d | Full Size: %d | ", index, index+1, size(), full_size());
                m_ptr_end = m_ptr_start + index + 1;
                recalcSize();
//                printf("New size: %d\n", size());

                return *(m_ptr_start + index);
            }

            return *(m_ptr_start + index);
        }

        /* ITERATION */

        Iterator begin(){
            return Iterator(m_ptr_start);
        };
        Iterator end(){
            return Iterator(m_ptr_end);
        };


    private:
        [[nodiscard]] inline size_t calcBlocksToRealloc(const size_t index) const {
            return m_AllocElementsAmount * std::max<size_t>((size_t)std::ceil(float(index) / float(m_AllocElementsAmount) + 1), 1);
        }

        inline void recalcSize(){
            m_size = m_ptr_end - m_ptr_start;
        }
    };

} // zly
