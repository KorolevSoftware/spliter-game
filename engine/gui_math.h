#pragma once
#include <cmath>
#pragma region Vec and Mat

using length_t = uint32_t;

// Vector
template<length_t N, typename T>
struct vec {
	T data[N];
	static constexpr length_t size = N;
};

template<template<length_t, typename> class Derived, length_t N, typename T>
struct vec_constructors_common {
	vec_constructors_common();
	explicit vec_constructors_common(T value);

	// Объявления методов копирования
	explicit vec_constructors_common(const vec_constructors_common& other) noexcept;
	Derived<N, T>& operator=(const vec_constructors_common& other) noexcept;

	template<typename... Args>
	explicit vec_constructors_common(const Args&... args);

private:
	// Вспомогательные методы
	template<typename... Args>
	static constexpr length_t count_elements();

	template<typename Arg>
	static constexpr length_t get_element_count();

	template<typename>
	struct is_vec : std::false_type {};

	template<length_t M, typename U>
	struct is_vec<vec<M, U>> : std::true_type {};

	void init_from_mixed(length_t offset);

	template<typename Arg, typename... Args>
	void init_from_mixed(length_t offset, const Arg& arg, const Args&... args);
};

template<typename T>
struct vec<2, T> : public vec_constructors_common<vec, 2, T> {
	union {
		T data[2];
		struct { T x, y; };
	};
	static constexpr length_t size = 2;
	using vec_constructors_common<vec, 2, T>::vec_constructors_common;
};

template<typename T>
struct vec<3, T>: public vec_constructors_common<vec, 3, T> {
	union {
		T data[3];
		struct { T x, y, z; };
	};
	static constexpr length_t size = 3;
	using vec_constructors_common<vec, 3, T>::vec_constructors_common;
};

template<typename T>
struct vec<4, T> : public vec_constructors_common<vec, 4, T> {
	union {
		T data[4];
		struct { T x, y, z, p; };
	};

	using vec_constructors_common<vec, 4, T>::vec_constructors_common;
};

template<template<length_t, typename> class Derived, length_t N, typename T>
template<typename... Args>
constexpr length_t vec_constructors_common<Derived, N, T>::count_elements() {
	return (get_element_count<Args>() + ...);
}

template<template<length_t, typename> class Derived, length_t N, typename T>
template<typename Arg>
constexpr length_t vec_constructors_common<Derived, N, T>::get_element_count() {
	if constexpr (is_vec<Arg>::value) {
		return Arg::size;
	} else {
		static_assert(std::is_convertible_v<Arg, T>,
			"Argument must be convertible to vector element type");
		return 1;
	}
}

template<template<length_t, typename> class Derived, length_t N, typename T>
void vec_constructors_common<Derived, N, T>::init_from_mixed(length_t) {}

template<template<length_t, typename> class Derived, length_t N, typename T>
template<typename Arg, typename... Args>
void vec_constructors_common<Derived, N, T>::init_from_mixed(
	length_t offset, const Arg& arg, const Args&... args) {
	auto& self = static_cast<Derived<N, T>&>(*this);

	if constexpr (is_vec<Arg>::value) {
		for (length_t i = 0; i < Arg::size; ++i) {
			self.data[offset + i] = arg.data[i];
		}
		init_from_mixed(offset + Arg::size, args...);
	} else {
		self.data[offset] = static_cast<T>(arg);
		init_from_mixed(offset + 1, args...);
	}
}

template<template<length_t, typename> class Derived, length_t N, typename T>
template<typename... Args>
vec_constructors_common<Derived, N, T>::vec_constructors_common(const Args&... args) {
	static_assert(count_elements<Args...>() == N,
		"Total number of elements must match vector size");
	init_from_mixed(0, args...);
}


// Matrix
using dim_t = uint32_t;
template<dim_t Size,  typename T>
struct mat {
	vec<Size, T> data[Size];
	mat();
};

#pragma endregion

// Подсчёт общего количества элементов на этапе компиляции
template<typename... Args>
static constexpr length_t count_elements() {
	return (get_element_count<Args>() + ...);
}

template<template<length_t, typename> class Derived, length_t N, typename T>
 vec_constructors_common<Derived, N, T>::vec_constructors_common() {
	auto& self = static_cast<Derived<N, T>&>(*this);
	for (length_t i = 0; i < N; ++i) {
		self.data[i] = T(0);
	}
}

template<template<length_t, typename> class Derived, length_t N, typename T>
vec_constructors_common<Derived, N, T>::vec_constructors_common(T value) {
	auto& self = static_cast<Derived<N, T>&>(*this);
	for (length_t i = 0; i < N; ++i) {
		self.data[i] = value;
	}
}

template<length_t N, typename type>
vec<N, type> operator+(const vec<N, type>& firts, const vec<N, type>& second) {
	vec<N, type> result;
	for (size_t i = 0; i < N; i++) {
		result.data[i] = firts.data[i] + second.data[i];
	}
	return result;
}

template<length_t N, typename type>
vec<N, type> operator-(const vec<N, type>& firts, const vec<N, type>& second) {
	vec<N, type> result;
	for (size_t i = 0; i < N; i++) {
		result.data[i] = firts.data[i] - second.data[i];
	}
	return result;
}

template<length_t N, typename type>
vec<N, type> operator/(const vec<N, type>& firts, const vec<N, type>& second) {
	vec<N, type> result;
	for (size_t i = 0; i < N; i++) {
		result.data[i] = firts.data[i] / second.data[i];
	}
	return result;
}

template<length_t N, typename type>
vec<N, type> operator*(const vec<N, type>& firts, const vec<N, type>& second) {
	vec<N, type> result;
	for (size_t i = 0; i < N; i++) {
		result.data[i] = firts.data[i] * second.data[i];
	}
	return result;
}

template<length_t N, typename T>
vec<N, T> operator*(const vec<N, T>& self, T scalar)  {
	vec<N, T> result;
	for (length_t i = 0; i < N; ++i) {
		result.data[i] = self.data[i] * scalar;
	}
	return result;
}

template<length_t N, typename T>
vec<N, T> operator/(const vec<N, T>& v, T scalar) {
	T inv_scalar = static_cast<T>(1) / scalar;  // Одно деление вместо N
	vec<N, T> result;
	for (length_t i = 0; i < N; ++i) {
		result.data[i] = v.data[i] * inv_scalar;
	}
	return result;
}

template<length_t N, typename T>
vec<N, T> operator/(T scalar, const vec<N, T>& v) {
	vec<N, T> result;
	for (length_t i = 0; i < N; ++i) {
		result.data[i] = scalar / v.data[i];
	}
	return result;
}

template<dim_t size, typename T>
mat<size, T>::mat() {
	for (size_t i = 0; i < size; i++) {
		data[i].data[i] = T(1);
	}
}

template<dim_t Size, typename T>
vec<Size, T> operator*(const mat<Size, T>& m, const vec<Size, T>& v) {
	vec<Size, T> result;

	for (dim_t i = 0; i < Size; ++i) {       // Цикл по строкам результата (i)
		T sum = 0;
		for (dim_t k = 0; k < Size; ++k) {   // Суммирование по столбцам матрицы (k)
			sum += m.data[k].data[i] * v.data[k];  // m(k,i) * v[k]
		}
		result.data[i] = sum;
	}

	return result;
}

// Реализация методов vec_constructors_common (вне класса)
template<template<length_t, typename> class Derived, length_t N, typename T>
vec_constructors_common<Derived, N, T>::vec_constructors_common(const vec_constructors_common& other) noexcept {
	auto& self = static_cast<Derived<N, T>&>(*this);
	const auto& other_derived = static_cast<const Derived<N, T>&>(other);

	if constexpr (std::is_trivially_copyable_v<T>) {
		std::memcpy(self.data, other_derived.data, N * sizeof(T));
	} else {
		for (length_t i = 0; i < N; ++i) {
			self.data[i] = other_derived.data[i];
		}
	}
}

template<template<length_t, typename> class Derived, length_t N, typename T>
Derived<N, T>& vec_constructors_common<Derived, N, T>::operator=(const vec_constructors_common& other) noexcept {
	auto& self = static_cast<Derived<N, T>&>(*this);
	const auto& other_derived = static_cast<const Derived<N, T>&>(other);

	if (this != &other) {
		if constexpr (std::is_trivially_copyable_v<T>) {
			std::memcpy(self.data, other_derived.data, N * sizeof(T));
		} else {
			for (length_t i = 0; i < N; ++i) {
				self.data[i] = other_derived.data[i];
			}
		}
	}
	return self;
}
template<typename T>
mat<4, T> translation(T tx, T ty, T tz) {
	mat<4, T> m;

	// Единичная матрица с добавленным смещением в последнем столбце
	m.data[0] = { 1, 0, 0, 0 };  // Первый столбец
	m.data[1] = { 0, 1, 0, 0 };  // Второй столбец
	m.data[2] = { 0, 0, 1, 0 };  // Третий столбец
	m.data[3] = { tx, ty, tz, 1 };  // Четвёртый столбец (вектор смещения)

	return m;
}

template<length_t N, typename T>
mat<4, T> translation(const vec<N, T>& position) {
	static_assert(N >= 3, "Too few arguments");
	mat<4, T> m;

	// Единичная матрица с добавленным смещением в последнем столбце
	m.data[0] = { 1, 0, 0, 0 };  // Первый столбец
	m.data[1] = { 0, 1, 0, 0 };  // Второй столбец
	m.data[2] = { 0, 0, 1, 0 };  // Третий столбец
	m.data[3] = { position.data[0], position.data[1], position.data[2], static_cast<T>(1) };  // Четвёртый столбец

	return m;
}
template<typename T>
mat<4, T> scaling(T sx, T sy, T sz) {
	mat<4, T> m;

	// Диагональная матрица с коэффициентами масштабирования
	m.data[0] = { sx, 0, 0, 0 };  // Первый столбец
	m.data[1] = { 0, sy, 0, 0 };  // Второй столбец
	m.data[2] = { 0, 0, sz, 0 };  // Третий столбец
	m.data[3] = { 0, 0, 0, static_cast<T>(1) };   // Четвёртый столбец

	return m;
}

template<length_t N, typename T>
mat<4, T> scaling(const vec<N, T>& scale) {
	static_assert(N >= 3, "Too few arguments");
	mat<4, T> m;

	// Диагональная матрица с коэффициентами масштабирования
	m.data[0] = { scale.data[0], 0, 0, 0 };  // Первый столбец
	m.data[1] = { 0, scale.data[1], 0, 0 };  // Второй столбец
	m.data[2] = { 0, 0, scale.data[2], 0 };  // Третий столбец
	m.data[3] = { 0, 0, 0, 1 };              // Четвёртый столбец

	return m;
}
template<typename T>
mat<4, T> rotationZ(T angle) {
	mat<4, T> m;
	T c = std::cos(angle);
	T s = std::sin(angle);

	constexpr T one = static_cast<T>(1);

	// Матрица вращения вокруг Z (column-major)
	m.data[0] = { c, s, 0, 0 };   // Первый столбец
	m.data[1] = { -s, c, 0, 0 };  // Второй столбец
	m.data[2] = { 0, 0, one, 0 };   // Третий столбец
	m.data[3] = { 0, 0, 0, one };   // Четвёртый столбец

	return m;
}

template<dim_t Size, typename T>
mat<Size, T> inverse(const mat<Size, T>& m) {
	mat<Size, T> result;
	mat<Size, T> temp = m;

	// Прямой ход метода Гаусса
	for (dim_t col = 0; col < Size; ++col) {
		// Выбор главного элемента (частичный выбор)
		dim_t max_row = col;
		for (dim_t row = col + 1; row < Size; ++row) {
			if (std::abs(temp.data[row].data[col]) >
				std::abs(temp.data[max_row].data[col])) {
				max_row = row;
			}
		}

		// Перестановка строк
		if (max_row != col) {
			std::swap(temp.data[col], temp.data[max_row]);
			std::swap(result.data[col], result.data[max_row]);
		}

		// Нормализация текущей строки
		T pivot = temp.data[col].data[col];
		for (dim_t j = 0; j < Size; ++j) {
			temp.data[col].data[j] /= pivot;
			result.data[col].data[j] /= pivot;
		}

		// Обнуление столбца в других строках
		for (dim_t row = 0; row < Size; ++row) {
			if (row != col && std::abs(temp.data[row].data[col]) >
				std::numeric_limits<T>::epsilon()) {
				T factor = temp.data[row].data[col];
				for (dim_t j = 0; j < Size; ++j) {
					temp.data[row].data[j] -= temp.data[col].data[j] * factor;
					result.data[row].data[j] -= result.data[col].data[j] * factor;
				}
			}
		}
	}

	return result;
}
template<typename T>
mat<4, T> inverse_affine(const mat<4, T>& m) {
	mat<4, T> result;
	// Блок поворота
	for (dim_t i = 0; i < 3; ++i) {
		for (dim_t j = 0; j < 3; ++j) {
			result.data[i].data[j] = m.data[j].data[i];
		}
	}

	result.data[3] = result * m.data[3] * T(-1);
	result.data[3].data[3] = T(1);
	return result;
}

template<dim_t Size, typename T>
mat<Size, T> operator*(const mat<Size, T>& a, const mat<Size, T>& b) {
	mat<Size, T> result;

	for (dim_t j = 0; j < Size; ++j) {         // Цикл по столбцам результата (j)
		for (dim_t i = 0; i < Size; ++i) {     // Цикл по строкам результата (i)
			T sum = 0;
			for (dim_t k = 0; k < Size; ++k) { // Суммирование произведений
				sum += a.data[k].data[i] * b.data[j].data[k]; // a(k,i) * b(j,k)
			}
			result.data[j].data[i] = sum;           // Запись в столбец j, строку i
		}
	}

	return result;
}

template<length_t N, length_t M, typename T>
vec<N, T> resize_vec(const vec<M, T>& src, T fill_value = T{}) {
	vec<N, T> result{};
	const length_t copy_count = std::min(N, M);

	for (length_t i = 0; i < copy_count; ++i) {
		result.data[i] = src.data[i];
	}
	for (length_t i = copy_count; i < N; ++i) {
		result.data[i] = fill_value;
	}

	return result;
}