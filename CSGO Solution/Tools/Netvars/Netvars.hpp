#pragma once

// include stl vector
#include <vector>

// include the game recv struct
#include "../../SDK/Utils/Recv.hpp"

// define base netvar struct
struct C_NetVar
{   
    std::string                 m_strName;
    RecvProp*                   m_pProp;
    uint32_t                    m_uOffset;
    std::vector< RecvProp* >    m_aChildProps;
    std::vector< C_NetVar >     m_aChildTables;
};

// define the netvar manager class
class C_NetVarManager
{
public:
    virtual uint32_t       GetNetvar( uint32_t uTable, uint32_t uVar );
    virtual void           Instance( );
    virtual RecvProp*      GetNetProp( std::string strTable, std::string strVar );
private:
    inline C_NetVar        ScanTable( RecvTable* clientClass );
    inline uint32_t        GetOffset( const C_NetVar& pTable, const uint32_t uPropName );
    inline RecvProp*       GetNetvarProp( const C_NetVar& pTable, const std::string strPropName );
protected:
    std::vector < C_NetVar > m_aNetVars;
};

inline C_NetVarManager* g_NetvarManager = new C_NetVarManager( );