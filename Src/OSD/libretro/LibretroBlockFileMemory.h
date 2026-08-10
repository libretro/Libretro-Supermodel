#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <BlockFile.h>

// ------------------------------------------------------------
// Counts how many bytes SaveState() would write
// ------------------------------------------------------------
class CBlockFileCounter : public CBlockFile
{
public:
    CBlockFileCounter();

    size_t GetSize() const;

    // EXACT signature matches
    void Write(const void* data, uint32_t numBytes) override;
    void Write(bool value) override;
    void Write(const std::string& str) override;
    void NewBlock(const std::string& name, const std::string& comment) override;

private:
    size_t m_size;
};

// ------------------------------------------------------------
// Writes SaveState() directly into a memory buffer
// ------------------------------------------------------------
class CBlockFileMemory : public CBlockFile
{
public:
    CBlockFileMemory(void* data, size_t size);
    void NewBlock(const std::string& name, const std::string& comment) override;
    Result FindBlock(const std::string &name) override;
    void Write(const void* data, uint32_t numBytes) override;
    void Write(bool value) override;
    void Write(const std::string& str) override;
    unsigned Read(void *data, uint32_t numBytes) override;
    unsigned Read(bool *value) override;
    void Finish();
private:
    uint8_t* m_ptr;
    size_t   m_offset;
    size_t   m_capacity;
    size_t m_currentBlockHeaderPos; // Add this to private members
};
