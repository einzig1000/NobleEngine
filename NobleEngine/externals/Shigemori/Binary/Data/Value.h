#pragma once
#include <string>
#include <vector>
#include <any>
#include <cassert>
#include <istream>
#include <ostream>
#include "definition/definition.h"

enum class TypeID : uint8_t {
	kUnknown = 0x00,	//不明な型

	//もともとある型
	Int = 0x01,
	Float,
	Bool,
	String,
	Double,
	uint8_t,
	uint16_t,
	uint32_t,

	//VectorとかMatrixとか
	Vector2 = 0x10,
	Vector3,
	Vector4,

	Vector = 0x40,

	//構造体とか
	Custom = 0x80,

};

template<typename T>
struct TypeIDResolver {
	static constexpr TypeID id = TypeID::kUnknown;
};

template<> struct TypeIDResolver<int> { static constexpr TypeID id = TypeID::Int; };
template<> struct TypeIDResolver<float> { static constexpr TypeID id = TypeID::Float; };
template<> struct TypeIDResolver<bool> { static constexpr TypeID id = TypeID::Bool; };
template<> struct TypeIDResolver<std::string> { static constexpr TypeID id = TypeID::String; };
template<> struct TypeIDResolver<double> { static constexpr TypeID id = TypeID::Double; };
template<> struct TypeIDResolver<std::vector<int>> { static constexpr TypeID id = TypeID::Vector; };
template<> struct TypeIDResolver<uint8_t> { static constexpr TypeID id = TypeID::uint8_t; };
template<> struct TypeIDResolver<uint16_t> { static constexpr TypeID id = TypeID::uint16_t; };
template<> struct TypeIDResolver<uint32_t> { static constexpr TypeID id = TypeID::uint32_t; };
template<> struct TypeIDResolver<Vector2> { static constexpr TypeID id = TypeID::Vector2; };
template<> struct TypeIDResolver<Vector3> { static constexpr TypeID id = TypeID::Vector3; };
template<> struct TypeIDResolver<Vector4> { static constexpr TypeID id = TypeID::Vector4; };

class ValueBase {
public:
	ValueBase(std::string name) : name(name) {}
	virtual ~ValueBase() = default;

	template<typename T>
	T get() {
		std::any buffer = GetValueData();

		//tryの中でエラーが発生したらキャッチに飛ぶやつ
		try {
			return std::any_cast<T>(buffer);
		} catch (const std::bad_any_cast& e) {
			assert(false && "ValueBase::get() failed: Type mismatch");
			e;
			return T(); // 型が一致しない場合はデフォルト値を返す
		}

	};

	virtual uint8_t GetTypeID() const = 0;

	virtual void Serialize(std::ostream& out) const = 0;
	virtual void Deserialize(std::istream& in) = 0;

	std::string name;		//変数名

protected:
	ValueBase() = default;

private:
	//std::anyはできれば使いたくないので、誰も使えないようにprivateにする

	//値を取得する
	virtual std::any GetValueData() = 0;
};

template<typename T>
class Value : public ValueBase {
public:
	Value(T value, std::string name = "default") : ValueBase(name), value(value) {};
	~Value() override = default;

	//値を変更する
	void set(const T& newValue) {
		value = newValue;
	};

	uint8_t GetTypeID() const override {
		return static_cast<uint8_t>(TypeIDResolver<T>::id);
	};

	void Serialize(std::ostream& out) const override {
		out.write(reinterpret_cast<const char*>(&value), sizeof(T));
	}

	void Deserialize(std::istream& in) override {
		in.read(reinterpret_cast<char*>(&value), sizeof(T));
	}

	T value;				//値

private:
	//親クラスが値を取得する用関数
	std::any GetValueData() override {
		return value;
	}
};
