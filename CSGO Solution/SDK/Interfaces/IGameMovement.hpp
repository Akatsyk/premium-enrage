#pragma once

class C_MoveData;
class C_BasePlayer;
class CGameMovement
{
public:
    virtual			          ~CGameMovement(void) {}
    virtual void	          ProcessMovement(C_BasePlayer *pPlayer, C_MoveData *pMove) = 0;
    virtual void	          Reset(void) = 0;
    virtual void	          StartTrackPredictionErrors(C_BasePlayer *pPlayer) = 0;
    virtual void	          FinishTrackPredictionErrors(C_BasePlayer *pPlayer) = 0;
    virtual void	          DiffPrint(char const *fmt, ...) = 0;
    virtual Vector const&	  GetPlayerMins(bool ducked) const = 0;
    virtual Vector const&	  GetPlayerMaxs(bool ducked) const = 0;
    virtual Vector const&     GetPlayerViewOffset(bool ducked) const = 0;
    virtual bool		      IsMovingPlayerStuck(void) const = 0;
    virtual C_BasePlayer*     GetMovingPlayer(void) const = 0;
    virtual void		      UnblockPusher(C_BasePlayer *pPlayer, C_BasePlayer *pPusher) = 0;
    virtual void              SetupMovementBounds(C_MoveData *pMove) = 0;
};

class C_GameMovement : public CGameMovement
{
public:
    virtual ~C_GameMovement(void) {}
};