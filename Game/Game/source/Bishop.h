#pragma once
#include "CharacterBase.h"

class Bishop :
    public CharacterBase
{
    typedef CharacterBase base;
public:
    Bishop();
    void Init();
    void CharacterObbSet();
    void Process();
    void PlayerProcess(float camVDir, VECTOR target);
    void EnemyProcess(VECTOR target);
    void Move();
    void EnemyMove();
    void Roll();
    void RollFront();
    void Attack();
    void ClassAttackLow();
    void ClassAttackHigh();
    void Damage();

    void Render();

protected:
    float _moveInRollSpd;
    
};

