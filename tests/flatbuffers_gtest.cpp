#include "monster_generated.h"
#include "gtest/gtest.h"

#include <fstream>
#include <string>

using namespace MyGame::Sample;

void WriteMonsterToFile(const std::string& filename) {
  // Build up a serialized buffer algorithmically:
  flatbuffers::FlatBufferBuilder builder;

  // First, lets serialize some weapons for the Monster: A 'sword' and an 'axe'.
  auto weapon_one_name = builder.CreateString("Sword");
  short weapon_one_damage = 3;

  auto weapon_two_name = builder.CreateString("Axe");
  short weapon_two_damage = 5;

  // Use the `CreateWeapon` shortcut to create Weapons with all fields set.
  auto sword = CreateWeapon(builder, weapon_one_name, weapon_one_damage);
  auto axe = CreateWeapon(builder, weapon_two_name, weapon_two_damage);

  // Create a FlatBuffer's `vector` from the `std::vector`.
  std::vector<flatbuffers::Offset<Weapon>> weapons_vector;
  weapons_vector.push_back(sword);
  weapons_vector.push_back(axe);
  auto weapons = builder.CreateVector(weapons_vector);

  // Second, serialize the rest of the objects needed by the Monster.
  auto position = Vec3(1.0f, 2.0f, 3.0f);

  auto name = builder.CreateString("MyMonster");

  unsigned char inv_data[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  auto inventory = builder.CreateVector(inv_data, 10);

  // Shortcut for creating monster with all fields set:
  auto orc = CreateMonster(builder, &position, 150, 80, name, inventory,
                           Color_Red, weapons, Equipment_Weapon, axe.Union());

  builder.Finish(orc); // Serialize the root of the object.

  // We now have a FlatBuffer we can store on disk or send over a network.
  std::ofstream outfile(filename, std::ios::binary);
  outfile.write((char *)builder.GetBufferPointer(), builder.GetSize());
  outfile.flush();
  outfile.close();
}

TEST(FlatbuffersTest, Monster) {
  WriteMonsterToFile("monster.bin");

  // ** file/network code goes here :) **
  std::ifstream infile;
  infile.open("monster.bin", std::ios::binary | std::ios::in);
  infile.seekg(0, std::ios::end);
  int length = infile.tellg();
  infile.seekg(0, std::ios::beg);
  char *data = new char[length];
  infile.read(data, length);
  infile.close();

  // Get access to the root:
  auto monster = GetMonster(data);
  delete[] data;

  // Get and test some scalar types from the FlatBuffer.
  EXPECT_EQ(monster->hp(), 80);
  EXPECT_EQ(monster->mana(), 150);
  EXPECT_EQ(monster->name()->str(), "MyMonster");

  // Get and test a field of the FlatBuffer's `struct`.
  auto pos = monster->pos();
  EXPECT_EQ(pos->z(), 3.0f);

  // Get a test an element from the `inventory` FlatBuffer's `vector`.
  auto inv = monster->inventory();
  EXPECT_EQ(inv->Get(9), 9);

  // Get and test the `weapons` FlatBuffers's `vector`.
  std::string expected_weapon_names[] = {"Sword", "Axe"};
  short expected_weapon_damages[] = {3, 5};
  auto weps = monster->weapons();
  for (unsigned int i = 0; i < weps->size(); i++) {
    EXPECT_EQ(weps->Get(i)->name()->str(), expected_weapon_names[i]);
    EXPECT_EQ(weps->Get(i)->damage(), expected_weapon_damages[i]);
  }

  // Get and test the `Equipment` union (`equipped` field).
  EXPECT_EQ(monster->equipped_type(), Equipment_Weapon);
  auto equipped = static_cast<const Weapon *>(monster->equipped());
  EXPECT_EQ(equipped->name()->str(), "Axe");
  EXPECT_EQ(equipped->damage(), 5);
}