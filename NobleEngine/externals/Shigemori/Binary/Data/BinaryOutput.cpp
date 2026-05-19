#include "BinaryOutput.h"

void BinaryOutput::WriteBinary(std::ostream& out, const ValueBase* value) {
	//型名を出力
	uint8_t typeID = value->GetTypeID();
	out.write(reinterpret_cast<const char*>(&typeID), sizeof(typeID));

	//変数名を出力
	uint8_t nameLength = static_cast<uint8_t>(value->name.size());
	out.write(reinterpret_cast<const char*>(&nameLength), sizeof(nameLength));
	out.write(value->name.data(), nameLength);

	if (value->GetTypeID() == uint8_t(TypeID::String)) {
		//配列のサイズを出力
		auto stringValue = dynamic_cast<const Value<std::string>*>(value);
		uint8_t stringLength = static_cast<uint8_t>(stringValue->value.size());
		out.write(reinterpret_cast<const char*>(&stringLength), sizeof(stringLength));
		out.write(stringValue->value.data(), stringLength);
	} else {
		//値を出力
		value->Serialize(out);
	}
}
