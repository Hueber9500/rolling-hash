#include <string>
#include "catch2/catch_test_macros.hpp"
#include "StringStream.h"
#include "Chunker.h"
#include "Comparer.h"
#include "FileStream.h"
#include <fstream>

const int base = 2287;
const int window_size = 4;

TEST_CASE( "String stream", "[byte_stream]"){
  StringStream stream("This is test string");

  REQUIRE(stream.length() == 19);
  char read[5] = {0};
  stream.readBytes(read, sizeof(read));
  REQUIRE(stream.isEndOfStream() == false);
  REQUIRE(strncmp(read, "This ", sizeof(read)) == 0);
  REQUIRE(stream.length() == 19);
  stream.readBytes(read, sizeof(read));
  REQUIRE(strncmp(read, "is te", sizeof(read)) == 0);
  stream.readBytes(read, sizeof(read));
  REQUIRE(strncmp(read, "st st", sizeof(read)) == 0);
  stream.readBytes(read, sizeof(read));
  REQUIRE(strncmp(read, "ring", sizeof(read)) == 0);
  stream.readBytes(read, sizeof(read));
  REQUIRE(strncmp(read, "ring", sizeof(read)) == 0);
  memset(read, 0, sizeof(read));
  REQUIRE(strncmp(read, "", sizeof(read)) == 0);
  REQUIRE(stream.isEndOfStream() == true);
}

TEST_CASE( "Test chunks comparison", "[comparer]"){
  ChunkInfo x(0,0,0,"","");
  std::vector<ChunkInfo> original_chunks = {
    { 
        ChunkInfo (10, 0, 0, "", "first_chunk"),
        ChunkInfo (10, 10, 0, "", "second chunk"),
        ChunkInfo (5, 20, 0, "", "third chunk"),
        ChunkInfo (20, 25, 0, "", "fourth chunk")
    }
  };

  std::vector<ChunkInfo> modified_chunks = {
    { 
       ChunkInfo (10, 0, 0, "", "first_chunk"),
       ChunkInfo (30, 10, 0, "", "second chunk modified"),
       ChunkInfo (5, 40, 0, "", "third chunk modified")
    }
  };

  auto result = Comparer().delta(original_chunks, modified_chunks, true);
  REQUIRE(result.size() == 5);

  result = Comparer().delta(original_chunks, modified_chunks, false);
  REQUIRE(result.size() == 6);

}


std::string originalText(){
  return "The fact that template type deduction deduces the “wrong” types for 0 and NULL (i.e., their true types, rather than their fallback meaning as a representation for a null pointer) is the most compelling reason to use nullptr instead of 0 or NULL when you want to refer to a null pointer. With nullptr, templates pose no special challenge. Combined with the fact that nullptr doesn’t suffer from the overload resolution sur prises that 0 and NULL are susceptible to, the case is ironclad.";
}

std::string modifiedText(){
  return "that template type deduction deduces the “wrong” types for 0 and NULL (i.e., their true types, rather than their fallback meaning as a representation for a null pointer) is the most compelling reason to use nullptr instead of 0 or NULL when you want to refer to a null pointer. With nullptr, templates pose no special challenge. Combined with the fact that nullptr doesn’t suffer from the overload resolution sur prises that 0 and NULL are susceptible to, the case is ironclad.";
}

TEST_CASE("Equal byte stream read", "[stream]"){
  std::fstream out_stream;

  out_stream.open("original_file.txt", std::ios_base::out);

  if(out_stream.is_open())
  {
      out_stream<<originalText();
      out_stream.flush();
      out_stream.close();
  }
  FileStream file_stream("original_file.txt");
  StringStream string_stream(originalText());

  REQUIRE(file_stream.length() == string_stream.length());

  while(!file_stream.isEndOfStream()){
    char file_stream_buffer[100] = {0};
    char string_stream_buffer[100] = {0};

    file_stream.readBytes(file_stream_buffer, sizeof(file_stream_buffer));
    string_stream.readBytes(string_stream_buffer, sizeof(string_stream_buffer));

    REQUIRE(strncmp(file_stream_buffer, string_stream_buffer, sizeof(file_stream_buffer)) == 0);
  }

  remove("original_file.txt");
}

TEST_CASE( "Test chunking", "[chunker]"){
  StringStream original_stream(originalText()), modified_stream(modifiedText());
  RollingHash rolling_hash(base, window_size);

  auto original_chunks = Chunker(&original_stream, &rolling_hash).chunkFile();
  rolling_hash.Init();
  auto modified_chunks = Chunker(&modified_stream, &rolling_hash).chunkFile();

  REQUIRE(original_chunks.size() == 29);
  REQUIRE(modified_chunks.size() == 28);

  auto differences = Comparer().delta(original_chunks, modified_chunks, true);

  REQUIRE(differences.size() == 3);
  REQUIRE(differences.at(0).action == Action::Insert);
  REQUIRE(differences.at(0).from_file == FileType::New);
  REQUIRE(differences.at(0).from_position == 0);
  REQUIRE(differences.at(0).to_position == 11);
}