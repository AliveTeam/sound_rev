#ifndef _SEQ_CHUNK_PARSER_HPP_
#define _SEQ_CHUNK_PARSER_HPP_

static inline unsigned int MakeType(unsigned char b4, unsigned char b3, unsigned char b2, unsigned char b1)
{
    return (static_cast<unsigned int>(b1) << 24) +
           (static_cast<unsigned int>(b2) << 16) +
           (static_cast<unsigned int>(b3) << 8) +
           static_cast<unsigned int>(b4);
}

struct ChunkHeader
{
    unsigned int iSize;     // Size inc header
    unsigned int iRefCount; // Ref count, should be zero when loaded
    unsigned int iType;
    unsigned int iId; // Id
};

class SeqChunkParser
{
public:
    SeqChunkParser()
     : mData(0), mNumSeqs(0)
    {
        
    }

    bool Parse(unsigned char *seqChunk)
    {
        mData = seqChunk;
        bool ok = true;
        mNumSeqs = 0;
        unsigned char *ptr = mData;
        for (;;)
        {
            ChunkHeader *chunk = reinterpret_cast<ChunkHeader *>(ptr);
            if (chunk->iType == MakeType('E', 'n', 'd', '!'))
            {
                printf("At end\n");
                break;
            }
            else if (chunk->iType == MakeType('S', 'e', 'q', ' '))
            {
                mNumSeqs++;
            }
            else
            {
                printf("Unknown chunk type\n");
            }
            ptr += chunk->iSize;
        }

        printf("Found %i SEQ's\n", mNumSeqs);

        return ok;
    }

    unsigned char *SeqPtr(unsigned char* pSeqBuffer, unsigned int index)
    {
        if (index > mNumSeqs)
        {
            return NULL;
        }

        unsigned char *ptr = pSeqBuffer;
        unsigned int count = 0;
        for (;;)
        {
            ChunkHeader *chunk = reinterpret_cast<ChunkHeader *>(ptr);
            if (chunk->iType == MakeType('E', 'n', 'd', '!'))
            {
                printf("Hit end chunk\n");
                return NULL;
            }
            else if (chunk->iType == MakeType('S', 'e', 'q', ' '))
            {
                if (count == index)
                {
                    printf("Found SEQ at index %d\n", index);
                    return reinterpret_cast<unsigned char *>(chunk);
                }
                count++;
            }
            ptr += chunk->iSize;
        }
        return NULL;
    }

    unsigned int NumSeqs() const
    {
        return mNumSeqs;
    }

private:
    unsigned char *mData;
    unsigned int mNumSeqs;
};

#endif // _SEQ_CHUNK_PARSER_HPP_
