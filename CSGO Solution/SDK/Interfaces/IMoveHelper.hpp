#pragma once

class C_ClientEntity;

class C_MoveHelper
{
public:
    virtual	void unused() = 0;
    virtual void SetHost( C_ClientEntity* host ) = 0;
};