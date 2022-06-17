#include "person.pb.h"
#include "pet.pb.h"

using cpcos::Person;
using cpcos::Pet;

int main(int argc, char* argv[])
{
  Person person;
  person.set_name("John");
  person.set_id(1);
  Pet pet;
  pet.set_name("cat");
  pet.set_id(1);
  return 0;
}
