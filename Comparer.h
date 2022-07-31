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

    DifferenceDescriptor(){
        memset(this, 0, sizeof(*this));
    }
};

class Comparer
{
    public:
        std::vector<DifferenceDescriptor> delta(const std::vector<ChunkInfo>& original, const std::vector<ChunkInfo>& modified, bool only_differences = false)
        {
            std::vector<DifferenceDescriptor> diff;
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
                current.from_position = it->offset;
                current.to_position = it->offset + it->length - 1;

                if( found ) 
                {
                    current.action = Action::Keep;
                    current.from_file = FileType::Old;
                } 
                else 
                {
                    current.action = Action::Insert;
                    current.from_file = FileType::New;
                }

                if ( only_differences ) {
                    if ( current.action != Action::Keep ) {
                        diff.push_back(std::move(current));
                    }
                } else {
                    diff.push_back(std::move(current));
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
                    current.to_position = it->offset + it->length - 1;

                    diff.push_back(std::move(current));
                }
            }

            return diff;
        }
};