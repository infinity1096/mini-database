#ifndef TEST_STREAM_IO_HPP
#define TEST_STREAM_IO_HPP


#include <map>
#include <sstream>
#include "Testable.hpp"

#include "Entity.hpp"
#include "Attribute.hpp"
#include "BlockIO.hpp"

namespace ECE141{

    class TestStreamIO : public Testable {
        public:
        using TestTestStreamIOFunc = bool (TestStreamIO::*)();

        std::map<std::string, TestTestStreamIOFunc> testFunctions {
            {"TestSaveEntity", &TestStreamIO::TestSaveEntity},
            {"TestSaveEntityMap", &TestStreamIO::testSaveEntityMap},
            {"TestSaveRow", &TestStreamIO::testSaveRow},
        };

        bool TestSaveEntity(){
            Entity class1("class1");
            Entity class2("class2");

            Attribute a[6];
            a[0].setName("A0");
            a[0].setDataType(DataTypes::varchar_type);
            
            a[1].setName("A1");
            a[1].setDataType(DataTypes::int_type);
            
            a[2].setName("A2");
            a[2].setDataType(DataTypes::float_type);
            
            a[3].setName("A3");
            a[3].setDataType(DataTypes::varchar_type);
            
            a[4].setName("A4");
            a[4].setDataType(DataTypes::float_type);
            
            a[5].setName("A5");
            a[5].setDataType(DataTypes::datetime_type);

            class1.addAttribute(a[0]);
            class1.addAttribute(a[1]);
            class1.addAttribute(a[2]);
            
            class2.addAttribute(a[3]);
            class2.addAttribute(a[4]);
            class2.addAttribute(a[5]);
            
            std::fstream buffer;
            buffer.open("C:/Users/yuche/Documents/GitHub/assignment-3-yuchen-ivan/temp/text2.txt", std::ios::binary | std::ios::in | std::ios::out | std::ios::trunc);
            StreamIO storage(buffer);

            bool success = true;

            success &= storage.listEntity().size() == 0;

            storage.saveEntity(class1);

            success &= storage.listEntity().size() == 1;
            success &= storage.loadEntity("class1").has_value();
            success &= !storage.loadEntity("class2").has_value();
            
            auto attributes = storage.loadEntity("class1").value().getAttributes();
            for (size_t i = 0; i < 3; i++){
                success &= attributes[i] == a[i];
            }

            storage.saveEntity(class2);

            success &= storage.listEntity().size() == 2;
            success &= storage.loadEntity("class1").has_value();
            success &= storage.loadEntity("class2").has_value();
            
            
            AttributeList attributes1 = storage.loadEntity("class1").value().getAttributes();
            for (size_t i = 0; i < 3; i++){
                success &= attributes[i] == a[i];
            }

            AttributeList attributes2 = storage.loadEntity("class2").value().getAttributes();
            for (size_t i = 0; i < 3; i++){
                success &= attributes2[i] == a[i+3];
            }

            return success;
        }

        bool testSaveEntityMap(){
            std::fstream buffer;
            buffer.open("C:/Users/yuche/Documents/GitHub/assignment-3-yuchen-ivan/temp/text2.txt", std::ios::binary | std::ios::in | std::ios::out | std::ios::trunc);
            StreamIO stream(buffer);

            stream.entity_map["AAA"] = 0;
            stream.entity_map["BBB"] = 0;
            
            stream.save_entity_map();
            stream.save_entity_map();
            stream.save_entity_map();
            stream.save_entity_map();
            
            return stream.block_io.getBlockCount() == 1;
        }

        bool testSaveRow(){
            std::fstream buffer;
            buffer.open("C:/Users/yuche/Documents/GitHub/assignment-3-yuchen-ivan/temp/text2.txt", std::ios::binary | std::ios::in | std::ios::out | std::ios::trunc);
            StreamIO stream(buffer);

            Entity people("People");
            Attribute name("name", DataTypes::varchar_type, 20);
            Attribute birthday("birthday", DataTypes::datetime_type, 0);
            Attribute age("age", DataTypes::int_type, 0);
            Attribute height("height", DataTypes::float_type, 0);

            Entity animal("Animal");
            Attribute type("type", DataTypes::varchar_type, 20);

            
            name.setPrimaryKey(true);
            name.setNullable(false);

            people.addAttribute(name).addAttribute(birthday).addAttribute(age).addAttribute(height);

            animal.addAttribute(type).addAttribute(age).addAttribute(height);

            Row row1;
            row1.addContent(MyBasicVarcharType("student A"));
            row1.addContent(MyBasicDateTimeType({2000,01,01,0,0,0}));
            row1.addContent(MyBasicIntType(12));
            row1.addContent(MyBasicFloatType(1.5));

            Row row2;
            row2.addContent(MyBasicVarcharType("student B"));
            row2.addContent(MyBasicDateTimeType({2001,01,01,0,0,0}));
            row2.addContent(MyBasicIntType(11));
            row2.addContent(MyBasicFloatType(1.4));
            
            Row row3;
            row3.addContent(MyBasicVarcharType("student C"));
            row3.addContent(MyBasicDateTimeType({2002,01,01,0,0,0}));
            row3.addContent(MyBasicIntType(10));
            row3.addContent(MyBasicFloatType(1.3));

            Row row2_1;
            row2_1.addContent(MyBasicVarcharType("animal A"));
            row2_1.addContent(MyBasicIntType(1));
            row2_1.addContent(MyBasicFloatType(0.5));
            
            Row row2_2;
            row2_2.addContent(MyBasicVarcharType("animal B"));
            row2_2.addContent(MyBasicIntType(2));
            row2_2.addContent(MyBasicFloatType(0.6));

            stream.saveEntity(people);
            stream.saveEntity(animal);

            stream.saveRow(row1, "People");
            stream.saveRow(row2, "People");
            stream.saveRow(row2_1, "Animal");
            stream.saveRow(row2_2, "Animal");
            stream.saveRow(row3, "People");

            stream.block_io.debugDump();

            stream.visitRow("People", [](const Row& aRow){
                std::cout << aRow.toString() << "\n";
            });

            std::cout << "------------------------\n";
            stream.visitRow("Animal", [](const Row& aRow) {
                std::cout << aRow.toString() << "\n";
            });


            return true;
        }

        // testable infrastructure
        TestStreamIO(){
            count = getNumTests();
        }

        size_t getNumTests(){
            return testFunctions.size();
        }
    
        std::optional<std::string> getTestName(size_t index) const {
            size_t current_index = 0;

            for (auto kv : testFunctions){
                if (index == current_index++){
                    return kv.first;
                }
            }

            return std::nullopt;
        }

        bool operator() (const std::string& testName){
            if (testFunctions.count(testName) > 0){
                return (this->*testFunctions[testName])();
            }else{
                return false;
            }
        }
    };

}

#endif