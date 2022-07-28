#pragma once
#include <vector>
#include "ChunkInfo.h"

enum Action{
    Delete = 0,
    Insert = 1,
    Keep = 2,
};

enum FileType{
    Old,
    New
};

struct DifferenceDescriptor
{
    FileType from_file;
    Action action;
    int from_position;
    int to_position;
};

class Comparer
{
    public:
        std::vector<DifferenceDescriptor> Delta(const std::vector<ChunkInfo>& original, const std::vector<ChunkInfo>& modified, bool only_differences = false)
        {
            std::vector<DifferenceDescriptor> delta;
            for(std::vector<ChunkInfo>::const_iterator it = modified.cbegin(); it!= modified.cend(); it++)
            {
                int found = false;

                for(std::vector<ChunkInfo>::const_iterator old_it = original.cbegin(); old_it != original.cend(); old_it++)
                {
                    if( strncmp(it->sha1_hex, old_it->sha1_hex, sizeof(it->sha1_hex)) == 0){
                        found = true;
                        break;
                    }
                }

                DifferenceDescriptor current;

                if( found ) 
                {
                    current.action = Action::Keep;
                    current.from_file = FileType::Old;
                    current.from_position = it->offset;
                    current.to_position = it->length;
                } 
                else 
                {
                    current.action = Action::Insert;
                    current.from_file = FileType::New;
                    current.from_position = it->offset;
                    current.to_position = it->length;
                }

                if ( only_differences ) {
                    if ( current.action != Action::Keep ) {
                        delta.push_back(std::move(current));
                    }
                } else {
                    delta.push_back(std::move(current));
                }
            }

            for(std::vector<ChunkInfo>::const_iterator it = original.cbegin(); it!= original.cend(); it++)
            {
                int found = false;

                for(std::vector<ChunkInfo>::const_iterator old_it = modified.cbegin(); old_it != modified.cend(); old_it++)
                {
                    if( strncmp(it->sha1_hex, old_it->sha1_hex, sizeof(it->sha1_hex)) == 0){
                        found = true;
                        break;
                    }
                }

                if( !found ) 
                {
                    DifferenceDescriptor current;

                    current.action = Action::Delete;
                    current.from_file = FileType::Old;
                    current.from_position = it->offset;
                    current.to_position = it->length;

                    delta.push_back(std::move(current));
                }
            }

            return delta;
        }
};