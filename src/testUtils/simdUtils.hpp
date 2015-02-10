#ifndef SIMDUTILS_HPP
#define SIMDUTILS_HPP

#include <vector>
#include <iterator>

#include "cm/global.hpp"


namespace EmbeddedMT {
	namespace test {
		template<typename ValueType, typename VectorType>
		class ValueVectorConversion {
			public:
				typedef typename std::vector<ValueType>::iterator valueType_iterator;
				typedef typename std::vector<ValueType>::const_iterator const_valueType_iterator;

				ValueVectorConversion(void);

				std::size_t sizeValueType(void) const;
				std::size_t sizeVectorType(void) const;

				ValueType& operator[](std::size_t idx) const;
				ValueType atValueType(std::size_t idx) const;
				// If sizeValueType() is not a multiple of the nb of value types that fit into the vector type, the MSB's are filled with additional zeros
				VectorType atVectorType(std::size_t idx) const;

				// Need to be separate functions in case ValueType == VectorType
				void pushBackValueType(const ValueType& newValue);
				void pushBackVectorType(const VectorType& newValue);

				// Iterators
				class VectorTypeIterator : public std::iterator<std::bidirectional_iterator_tag, VectorType> {
					public:
						VectorTypeIterator(ValueVectorConversion<ValueType, VectorType>* valueVectorConversion);
						explicit VectorTypeIterator(ValueVectorConversion<ValueType, VectorType>* valueVectorConversion, std::size_t index);
						bool operator==(const VectorTypeIterator& rhs) const;
						bool operator!=(const VectorTypeIterator& rhs) const;
						VectorTypeIterator& operator++(void);
						VectorTypeIterator operator++(int forwardValue);
						VectorType operator*(void);
					private:
						bool isAtTheEnd(void) const;

						std::size_t _index;
						ValueVectorConversion<ValueType, VectorType>* _valueVectorConversion;
				};

				valueType_iterator valueTypeBegin(void);
			    valueType_iterator valueTypeEnd(void);
				VectorTypeIterator vectorTypeBegin(void);
				VectorTypeIterator vectorTypeEnd(void);
				
			private:
				const ValueType _valueTypesPerVector;
				std::vector<ValueType> _data;
		};


	}

	// Function definitions
	namespace test {
		template<typename ValueType, typename VectorType>
		ValueVectorConversion<ValueType, VectorType>::ValueVectorConversion() :
			_valueTypesPerVector(sizeof(VectorType)/sizeof(ValueType))
		{
			// Make sure that the ValueType fits an integer number of times in the VectorType
			STATIC_ASSERT(sizeof(VectorType) >= sizeof(ValueType), "The VectorType should be equal or greater than the ValueType");
			STATIC_ASSERT(sizeof(VectorType) % sizeof(ValueType) == 0, "The SIMD type should be an integer multiple of the actual type");
		}

		template<typename ValueType, typename VectorType>
		ValueType& ValueVectorConversion<ValueType, VectorType>::operator[](std::size_t idx) const {
			return atValueType(idx);
		}

		template<typename ValueType, typename VectorType>
		ValueType ValueVectorConversion<ValueType, VectorType>::atValueType(std::size_t idx) const {
			return _data[idx];
		}

		template<typename ValueType, typename VectorType>
		VectorType ValueVectorConversion<ValueType, VectorType>::atVectorType(std::size_t idx) const {
			std::size_t valueIdx = idx * _valueTypesPerVector;
			VectorType resultVector = 0;

			for(int32_t i = _valueTypesPerVector-1; i >= 0; --i) {
				// If there is still data left that can be fit into the type, add to the end of the type
				if(valueIdx + i < sizeValueType()) {
					ValueType dataValue = _data[valueIdx + i];
					resultVector <<= sizeof(ValueType) * GBL::NB_BITS_IN_A_BYTE;
					resultVector += dataValue;
				}
			}
			return resultVector;
		}

		template<typename ValueType, typename VectorType>
		void ValueVectorConversion<ValueType, VectorType>::pushBackValueType(const ValueType& newValue) {
			_data.push_back(newValue);
		}

		template<typename ValueType, typename VectorType>
		void ValueVectorConversion<ValueType, VectorType>::pushBackVectorType(const VectorType& newValue) {
			const ValueType* const valuePtr = reinterpret_cast<const ValueType*>(&newValue); 
			for(uint8_t i = 0; i < _valueTypesPerVector; ++i) {
				pushBackValueType(valuePtr[i]);
			}
		}

		template<typename ValueType, typename VectorType>
		std::size_t ValueVectorConversion<ValueType, VectorType>::sizeValueType(void) const {
			return _data.size();
		}

		template<typename ValueType, typename VectorType>
		std::size_t ValueVectorConversion<ValueType, VectorType>::sizeVectorType(void) const {
			std::size_t dataSize = sizeValueType();
			if(dataSize % _valueTypesPerVector == 0) {
				// If the size is a multiple of the nb of value types in a vector, return the size divided by this nb
				return dataSize / _valueTypesPerVector;
			} else {
				// Else return the size divided by this nb + 1
				return (dataSize / _valueTypesPerVector) + 1U;
			}
		}

		template<typename ValueType, typename VectorType>
		typename ValueVectorConversion<ValueType, VectorType>::valueType_iterator ValueVectorConversion<ValueType, VectorType>::valueTypeBegin() {
			return _data.begin();
		}

		template<typename ValueType, typename VectorType>
		typename ValueVectorConversion<ValueType, VectorType>::valueType_iterator ValueVectorConversion<ValueType, VectorType>::valueTypeEnd() {
			return _data.end();
		}

		template<typename ValueType, typename VectorType>
		typename ValueVectorConversion<ValueType, VectorType>::VectorTypeIterator ValueVectorConversion<ValueType, VectorType>::vectorTypeBegin() {
			return VectorTypeIterator(this, 0);
		}

		template<typename ValueType, typename VectorType>
		typename ValueVectorConversion<ValueType, VectorType>::VectorTypeIterator ValueVectorConversion<ValueType, VectorType>::vectorTypeEnd() {
			return VectorTypeIterator(this, sizeVectorType());
		}

		// Vector iterator stuff
		template<typename ValueType, typename VectorType>
		ValueVectorConversion<ValueType, VectorType>::VectorTypeIterator::VectorTypeIterator(ValueVectorConversion<ValueType, VectorType>* valueVectorConversion) : 
			_index(0), _valueVectorConversion(valueVectorConversion) {
			;
		}

		template<typename ValueType, typename VectorType>
		ValueVectorConversion<ValueType, VectorType>::VectorTypeIterator::VectorTypeIterator(ValueVectorConversion<ValueType, VectorType>* valueVectorConversion, std::size_t index) : 
			_index(index), _valueVectorConversion(valueVectorConversion) {
				;
		}

		template<typename ValueType, typename VectorType>
		bool ValueVectorConversion<ValueType, VectorType>::VectorTypeIterator::operator==(const VectorTypeIterator& rhs) const {
			// All iterators saying that the iterator has ended are considered to be equal
			if(isAtTheEnd() && rhs.isAtTheEnd()) {
				return true;
			} else {
				return (_index == rhs._index && isAtTheEnd() == rhs.isAtTheEnd() && _valueVectorConversion == rhs._valueVectorConversion);
			}
		}
		
		template<typename ValueType, typename VectorType>
		bool ValueVectorConversion<ValueType, VectorType>::VectorTypeIterator::operator!=(const VectorTypeIterator& rhs) const {
				return !(*this == rhs);
		}

		template<typename ValueType, typename VectorType>
		bool ValueVectorConversion<ValueType, VectorType>::VectorTypeIterator::isAtTheEnd() const {
			return _index >= _valueVectorConversion->sizeVectorType();
		}

		template<typename ValueType, typename VectorType>
		typename ValueVectorConversion<ValueType, VectorType>::VectorTypeIterator& ValueVectorConversion<ValueType, VectorType>::VectorTypeIterator::operator++() {
			++_index;
			return *this;
		}

		template<typename ValueType, typename VectorType>
		typename ValueVectorConversion<ValueType, VectorType>::VectorTypeIterator ValueVectorConversion<ValueType, VectorType>::VectorTypeIterator::operator++(int forwardValue) {
		  VectorTypeIterator tmp(*this); 
		  _index += forwardValue; 
		  return tmp;
		}

		template<typename ValueType, typename VectorType>
		VectorType ValueVectorConversion<ValueType, VectorType>::VectorTypeIterator::operator*() {
			if(! isAtTheEnd()) {
				return _valueVectorConversion->atVectorType(_index);
			}
			LOG_ERROR("Error: trying to dereference an element that is out of scope");
			assert(false);
			return _valueVectorConversion->atVectorType(_valueVectorConversion->sizeVectorType() - 1U);
		}
	}
}

#endif
