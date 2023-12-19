#pragma once
#include "CharacterBase.h"

class King :
    public CharacterBase
{
    typedef CharacterBase base;
public:
    King();
    void Init();
    void Process();
    void CharacterObbSet();
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

