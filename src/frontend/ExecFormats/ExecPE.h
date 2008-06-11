/* Copyright 2008 Lawrence Livermore National Security, LLC */
#ifndef Exec_ExecPE_h
#define Exec_ExecPE_h

#include "ExecGeneric.h"

namespace Exec {
namespace PE {

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// MS-DOS Real Mode File Header
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* File format of an MS-DOS Real Mode File Header. All fields are little endian. */
struct DOSFileHeader_disk {
    unsigned char e_magic[2];           /* 0x54, 0xAD */
    uint16_t    e_cblp;                 /* bytes on last page of file */
    uint16_t    e_cp;                   /* number of pages in file */
    uint16_t    e_crlc;                 /* relocations */
    uint16_t    e_cparhdr;              /* header size in paragraphs */
    uint16_t    e_minalloc;             /* number of extra paragraphs needed */
    uint16_t    e_maxalloc;
    uint16_t    e_ss;                   /* initial relative SS value */
    uint16_t    e_sp;                   /* initial relative SP value */
    uint16_t    e_csum;                 /* checksum */
    uint16_t    e_ip;                   /* initial IP value */
    uint16_t    e_cs;                   /* initial relative CS value */
    uint16_t    e_lfarlc;               /* file address of relocation table */
    uint16_t    e_ovno;                 /* overlay number */
    uint16_t    e_res1[4];              /* reserved */
    uint16_t    e_oemid;                /* OEM Identifier */
    uint16_t    e_oeminfo;              /* other OEM information; oemid specific */
    uint16_t    e_res2[10];             /* reserved */
    uint32_t    e_lfanew;               /* file offset of new exe (PE) header */
};

class DOSFileHeader : public ::Exec::ExecHeader {
  public:
    DOSFileHeader(ExecFile *f, addr_t offset)
        : ExecHeader(f, offset, sizeof(DOSFileHeader_disk)) {ctor(f, offset);}
    virtual void dump(FILE*, const char *prefix);

    /* These are the native-format versions of the same members described in the DOSFileHeader_disk format struct */
    unsigned            e_cblp, e_cp, e_crlc, e_cparhdr, e_minalloc, e_maxalloc, e_ss, e_sp, e_csum, e_ip, e_cs, e_lfarlc, e_ovno;
    unsigned            e_res1[4], e_oemid, e_oeminfo, e_res2[10], e_lfanew;
    
  private:
    void ctor(ExecFile *f, addr_t offset);
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PE File Header
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* File format for an RVA/Size pair. Such pairs are considered to be part of the PE file header. All fields are little endian. */
struct RVASizePair_disk {
    uint32_t    e_rva;
    uint32_t    e_size;
};

class RVASizePair {
  public:
    RVASizePair(const RVASizePair_disk *disk) {
        e_rva  = le_to_host(disk->e_rva);
        e_size = le_to_host(disk->e_size);
    }
    addr_t      e_rva, e_size;
};

/* File format of a PE File Header. All fields are little endian. */
struct PEFileHeader_disk {
    unsigned char e_magic[4];           /* magic number "PE\0\0" */
    uint16_t    e_cpu_type;             /* e.g., 0x014c = Intel 386 */
    uint16_t    e_nobjects;             /* number of objects (segments) defined in the Object Table */
    uint32_t    e_time;                 /* time and date file was created or modified by the linker */
    uint32_t    e_reserved1;
    uint32_t    e_reserved2;
    uint16_t    e_nt_hdr_size;          /* number of remaining bytes in the header following the 'flags' field */
    uint16_t    e_flags;                /* Bit flags: executable file, program/library image, fixed address, etc. */
    uint16_t    e_reserved3;
    uint16_t    e_lmajor;               /* linker version */
    uint16_t    e_lminor;
    uint16_t    e_reserved4;
    uint32_t    e_reserved5;
    uint32_t    e_reserved6;
    uint32_t    e_entrypoint_rva;       /* RVA="relative virtual address"; relative to 'image_base', below */
    uint32_t    e_reserved7;
    uint32_t    e_reserved8;
    uint32_t    e_image_base;           /* Virtual base of the image (first byte of file, DOS header). Multiple of 64k. */
    uint32_t    e_object_align;         /* Alignment of Objects in memory. Power of two 512<=x<=256M */
    uint32_t    e_file_align;           /* Alignment factor (in bytes) for image pages */
    uint16_t    e_os_major;             /* OS version number required to run this image */
    uint16_t    e_os_minor;
    uint16_t    e_user_major;           /* User-specified at link time. Useful for differentiating between image revisions */
    uint16_t    e_user_minor;
    uint16_t    e_subsys_major;         /* Subsystem version number */
    uint16_t    e_subsys_minor;
    uint32_t    e_reserved9;
    uint32_t    e_image_size;           /* Virtual size (bytes) of the image inc. all headers; multiple of 'object_align' */
    uint32_t    e_header_size;          /* Total header size (DOS Header + PE Header + Object table */
    uint32_t    e_file_checksum;        /* Checksum for entire file; Set to zero by the linker */
    uint16_t    e_subsystem;            /* Unknown, Native, WindowsGUI, WindowsCharacter, OS/2 Character, POSIX Character */
    uint16_t    e_dll_flags;            /* Bit flags for library init/terminate per process or thread */
    uint32_t    e_stack_reserve_size;   /* Virtual memory reserved for stack; non-committed pages are guards */
    uint32_t    e_stack_commit_size;    /* Size (bytes) of valid stack; other pages are guards; <= 'stack_reserve_size' */
    uint32_t    e_heap_reserve_size;    /* Size (bytes) of local heap to reserve */
    uint32_t    e_heap_commit_size;     /* Size (bytes) of valid local heap */
    uint32_t    e_reserved10;
    uint32_t    e_num_rvasize_pairs;    /* Number of RVASizePair entries that follow this member; also part of the PE header */
    /* RVA/Size pairs follow, typically for:
     *     - export table
     *     - import table
     *     - resource table
     *     - exception table
     *     - security table
     *     - fixup table
     *     - debug table
     *     - image description
     *     - machine specific
     *     - thread local storage */
    
};

/* Bit flags for the PE header 'flags' member */
enum HeaderFlags {
    HF_PROGRAM          = 0x0000,       /* Program image (no non-reserved bits set) */
    HF_EXECUTABLE       = 0x0002,       /* Clear indicates can't load: either link errors or image is being incrementally linked */
    HF_FIXED            = 0x0200,       /* Image *must* be loaded at image_base address or error */
    HF_LIBRARY          = 0x2000,       /* Library image */
    HF_RESERVED_MASK    = 0xddfd        /* Reserved bits */
};

/* Values for the PE header 'subsystem' member */
enum Subsystem {
    HF_SPEC_UNKNOWN     = 0x0000,       /* Specified as 'unknown' in the file */
    HF_NATIVE           = 0x0001,       /* Native */
    HF_WINGUI           = 0x0002,       /* Windows GUI */
    HF_WINCHAR          = 0x0003,       /* Windows character */
    HF_OS2CHAR          = 0x0005,       /* OS/2 character */
    HF_POSIX            = 0x0007        /* POSIX character */
};

/* Bit flags for the PE header 'dll_flags' member */
enum DLLFlags {
    DLL_PROC_INIT       = 0x0001,       /* Per-process library initialization */
    DLL_PROC_TERM       = 0x0002,       /* Per-process library termination */
    DLL_THRD_INIT       = 0x0004,       /* Per-thread library initialization */
    DLL_THRD_TERM       = 0x0008,       /* Per-thread library termination */
    DLL_RESERVED_MASK   = 0xfff0        /* Reserved bits */
};

class PEFileHeader : public ExecHeader {
  public:
    PEFileHeader(ExecFile *f, addr_t offset)
        : ExecHeader(f, offset, sizeof(PEFileHeader_disk)) {ctor(f, offset);}
    void add_rvasize_pairs();
    virtual void dump(FILE*, const char *prefix);
    
    /* These are the native-format versions of the same members described in the PEFileHeader_disk format struct. */
    unsigned    e_cpu_type, e_nobjects, e_time, e_reserved1, e_reserved2, e_nt_hdr_size, e_flags, e_reserved3;
    unsigned    e_lmajor, e_lminor, e_reserved4, e_reserved5, e_reserved6, e_entrypoint_rva, e_reserved7, e_reserved8;
    unsigned    e_image_base, e_object_align, e_file_align, e_os_major, e_os_minor, e_user_major, e_user_minor;
    unsigned    e_subsys_major, e_subsys_minor, e_reserved9, e_image_size, e_header_size, e_file_checksum, e_subsystem;
    unsigned    e_dll_flags, e_stack_reserve_size, e_stack_commit_size, e_heap_reserve_size, e_heap_commit_size;
    unsigned    e_reserved10, e_num_rvasize_pairs;
    std::vector<RVASizePair> rvasize_pairs;

  private:
    void ctor(ExecFile *f, addr_t offset);
};
    
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PE Object Table
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* File format of an object table entry. All fields are little endian. Objects are ordered by RVA. */
struct ObjectTableEntry_disk {
    char        name[8];                /* NUL-padded */
    uint32_t    virtual_size;           /* virtual memory size (bytes), >= physical_size and difference is zero filled */
    uint32_t    rva;                    /* relative virtual address wrt Image Base; multiple of object_align; dense space */
    uint32_t    physical_size;          /* bytes of initialized data on disk; multiple of file_align & <= virtual_size */
    uint32_t    physical_offset;        /* location of initialized data on disk; multiple of file_align */
    uint32_t    reserved[3];
    uint32_t    flags;                  /* ObjectFlags bits: code, data, caching, paging, shared, permissions, etc. */
};

/* These come from the windows PE documentation and http://en.wikibooks.org/wiki/X86_Disassembly/Windows_Executable_Files */
enum ObjectFlags {
    OF_CODE             = 0x00000020,   /* section contains code */
    OF_IDATA            = 0x00000040,   /* initialized data */
    OF_UDATA            = 0x00000080,   /* uninitialized data */
    OF_INFO             = 0x00000200,   /* comments or some other type of info */
    OF_REMOVE           = 0x00000800,   /* section will not become part of image */
    OF_COMDAT           = 0x00001000,   /* section contains comdat */
    OF_NO_DEFER_SPEC_EXC= 0x00004000,   /* reset speculative exception handling bits in the TLB entires for this section */
    OF_GPREL            = 0x00008000,   /* section content can be access relative to GP */
    OF_ALIGN_1          = 0x00100000,   /* no alignment */
    OF_ALIGN_2          = 0x00200000,   /* 2-byte alignment */
    OF_ALIGN_4          = 0x00300000,   /* 4-byte alignment */
    OF_ALIGN_8          = 0x00400000,   /* 8-byte alignment */
    OF_ALIGN_16         = 0x00500000,   /* 16-byte alignment (default if not other alignment specified) */
    OF_ALIGN_32         = 0x00600000,   /* 32-byte alignment */
    OF_ALIGN_64         = 0x00700000,   /* 64-byte alignment */
    OF_ALIGN_128        = 0x00800000,   /* 128-byte alignment */
    OF_ALIGN_256        = 0x00900000,   /* 256-byte alignment */
    OF_ALIGN_512        = 0x00a00000,   /* 512-byte alignment */
    OF_ALIGN_1k         = 0x00b00000,   /* 1024-byte alignment */
    OF_ALIGN_2k         = 0x00c00000,   /* 2048-byte alignment */
    OF_ALIGN_4k         = 0x00d00000,   /* 4096-byte alignment */
    OF_ALIGN_8k         = 0x00e00000,   /* 8192-byte alignment */
    OF_ALIGN_MASK       = 0x00f00000,   /* mask for alignment value */
    OF_NRELOC_OVFL      = 0x01000000,   /* section contains extended relocations */
    OF_DISCARDABLE      = 0x02000000,   /* can be discarded */
    OF_NO_CACHE         = 0x04000000,   /* object must not be cached */
    OF_NO_PAGING        = 0x08000000,   /* object is not pageable */
    OF_SHARED           = 0x10000000,   /* object is shared */
    OF_EXECUTABLE       = 0x20000000,   /* execute permission */
    OF_READABLE         = 0x40000000,   /* read permission */
    OF_WRITABLE         = 0x80000000,   /* write permission */
};

class ObjectTableEntry {
  public:
    ObjectTableEntry(const ObjectTableEntry_disk *disk)
        {ctor(disk);}
    virtual ~ObjectTableEntry() {};
    virtual void dump(FILE*, const char *prefix);
    
    /* These are the native-format versions of the same members described in the ObjectTableEntry_disk struct. */
    std::string name;
    addr_t      virtual_size, rva, physical_size, physical_offset;
    unsigned    reserved[3], flags;

  private:
    void ctor(const ObjectTableEntry_disk*);
};

class ObjectTable : public ExecSection {
  public:
    ObjectTable(PEFileHeader *fhdr)
        : ExecSection(fhdr->get_file(), fhdr->end_offset(), fhdr->e_nobjects*sizeof(ObjectTableEntry_disk)) {ctor(fhdr);}
    virtual void dump(FILE*, const char *prefix);
  private:
    void ctor(PEFileHeader*);
    std::vector<ObjectTableEntry*> entries;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PE Import Directory (".idata" segment)
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct ImportDirectory_disk {
    uint32_t            hintnames_rva;          /* address (RVA) of array of addresses (RVAs) of hint/name pairs */
    uint32_t            time;
    uint32_t            forwarder_chain;
    uint32_t            dll_name_rva;           /* address of NUL-terminated library name */
    uint32_t            bindings_rva;           /* address (RVA) of array of object addresses after binding to DLL */
};

class ImportDirectory {
  public:
    ImportDirectory(const ImportDirectory_disk *disk) {ctor(disk);}
    void dump(FILE*, const char *prefix);
    addr_t              hintnames_rva, bindings_rva, dll_name_rva;
    time_t              time;
    unsigned            forwarder_chain;
  private:
    void ctor(const ImportDirectory_disk*);
};

class ImportSegment : public ExecSegment {
  public:
    ImportSegment(ExecSection *section, addr_t offset, addr_t size, addr_t rva, addr_t mapped_size)
        : ExecSegment(section, offset, size, rva, mapped_size)
        {ctor(section, offset, size, rva, mapped_size);}
    virtual void dump(FILE*, const char *prefix);
  private:
    void ctor(ExecSection *section, addr_t offset, addr_t size, addr_t rva, addr_t mapped_size);
    std::vector<ImportDirectory*> dirs;
};


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/* Functions */
bool is_PE(ExecFile*);
void parse(ExecFile*);
    
}; //namespace PE
}; //namespace Exec

#endif /*!Exec_ExecPE_h*/
