#include "OrientedBoundingBox.h"
#include <math.h>
// OBB v.s. OBB
bool OBB::ColOBBs(OBB& obb1, OBB& obb2)
{
    if (obb1._effectiveTime >= 1 ||
        obb2._effectiveTime >= 1) {
        return false;
    }
    // 各方向ベクトルの確保
    // （N***:標準化方向ベクトル）
    VECTOR NAe1 = obb1.GetDirect(0), Ae1 = Math::VMulti(NAe1, obb1.GetLen_W(0));
    VECTOR NAe2 = obb1.GetDirect(1), Ae2 = Math::VMulti(NAe2, obb1.GetLen_W(1));
    VECTOR NAe3 = obb1.GetDirect(2), Ae3 = Math::VMulti(NAe3, obb1.GetLen_W(2));
    VECTOR NBe1 = obb2.GetDirect(0), Be1 = Math::VMulti(NBe1, obb2.GetLen_W(0));
    VECTOR NBe2 = obb2.GetDirect(1), Be2 = Math::VMulti(NBe2, obb2.GetLen_W(1));
    VECTOR NBe3 = obb2.GetDirect(2), Be3 = Math::VMulti(NBe3, obb2.GetLen_W(2));
    VECTOR Interval = VSub(obb1.GetPos_W(), obb2.GetPos_W());

    // 分離軸 : Ae1
    FLOAT rA = VSize(Ae1);
    FLOAT rB = LenSegOnSeparateAxis(NAe1, Be1, Be2, &Be3);
    FLOAT L = fabs(VDot(Interval, NAe1));
    if (L > rA + rB)
        return false; // 衝突していない

    // 分離軸 : Ae2
    rA = VSize(Ae2);
    rB = LenSegOnSeparateAxis(NAe2, Be1, Be2, &Be3);
    L = fabs(VDot(Interval, NAe2));
    if (L > rA + rB)
        return false;

    // 分離軸 : Ae3
    rA = VSize(Ae3);
    rB = LenSegOnSeparateAxis(NAe3, Be1, Be2, &Be3);
    L = fabs(VDot(Interval, NAe3));
    if (L > rA + rB)
        return false;

    // 分離軸 : Be1
    rA = LenSegOnSeparateAxis(NBe1, Ae1, Ae2, &Ae3);
    rB = VSize(Be1);
    L = fabs(VDot(Interval, NBe1));
    if (L > rA + rB)
        return false;

    // 分離軸 : Be2
    rA = LenSegOnSeparateAxis(NBe2, Ae1, Ae2, &Ae3);
    rB = VSize(Be2);
    L = fabs(VDot(Interval, NBe2));
    if (L > rA + rB)
        return false;

    // 分離軸 : Be3
    rA = LenSegOnSeparateAxis(NBe3, Ae1, Ae2, &Ae3);
    rB = VSize(Be3);
    L = fabs(VDot(Interval, NBe3));
    if (L > rA + rB)
        return false;

    // 分離軸 : C11
    VECTOR Cross;
    Cross = VCross(NAe1, NBe1);
    rA = LenSegOnSeparateAxis(Cross, Ae2, Ae3);
    rB = LenSegOnSeparateAxis(Cross, Be2, Be3);
    L = fabs(VDot(Interval, Cross));
    if (L > rA + rB)
        return false;

    // 分離軸 : C12
    Cross = VCross(NAe1, NBe2);
    rA = LenSegOnSeparateAxis(Cross, Ae2, Ae3);
    rB = LenSegOnSeparateAxis(Cross, Be1, Be3);
    L = fabs(VDot(Interval, Cross));
    if (L > rA + rB)
        return false;

    // 分離軸 : C13
    Cross = VCross(NAe1, NBe3);
    rA = LenSegOnSeparateAxis(Cross, Ae2, Ae3);
    rB = LenSegOnSeparateAxis(Cross, Be1, Be2);
    L = fabs(VDot(Interval, Cross));
    if (L > rA + rB)
        return false;

    // 分離軸 : C21
    Cross = VCross(NAe2, NBe1);
    rA = LenSegOnSeparateAxis(Cross, Ae1, Ae3);
    rB = LenSegOnSeparateAxis(Cross, Be2, Be3);
    L = fabs(VDot(Interval, Cross));
    if (L > rA + rB)
        return false;

    // 分離軸 : C22
    Cross = VCross(NAe2, NBe2);
    rA = LenSegOnSeparateAxis(Cross, Ae1, Ae3);
    rB = LenSegOnSeparateAxis(Cross, Be1, Be3);
    L = fabs(VDot(Interval, Cross));
    if (L > rA + rB)
        return false;

    // 分離軸 : C23
    Cross = VCross(NAe2, NBe3);
    rA = LenSegOnSeparateAxis(Cross, Ae1, Ae3);
    rB = LenSegOnSeparateAxis(Cross, Be1, Be2);
    L = fabs(VDot(Interval, Cross));
    if (L > rA + rB)
        return false;

    // 分離軸 : C31
    Cross = VCross(NAe3, NBe1);
    rA = LenSegOnSeparateAxis(Cross, Ae1, Ae2);
    rB = LenSegOnSeparateAxis(Cross, Be2, Be3);
    L = fabs(VDot(Interval, Cross));
    if (L > rA + rB)
        return false;

    // 分離軸 : C32
    Cross = VCross(NAe3, NBe2);
    rA = LenSegOnSeparateAxis(Cross, Ae1, Ae2);
    rB = LenSegOnSeparateAxis(Cross, Be1, Be3);
    L = fabs(VDot(Interval, Cross));
    if (L > rA + rB)
        return false;

    // 分離軸 : C33
    Cross = VCross(NAe3, NBe3);
    rA = LenSegOnSeparateAxis(Cross, Ae1, Ae2);
    rB = LenSegOnSeparateAxis(Cross, Be1, Be2);
    L = fabs(VDot(Interval, Cross));
    if (L > rA + rB)
        return false;

    // 分離平面が存在しないので「衝突している」
    return true;
}


// 分離軸に投影された軸成分から投影線分長を算出
FLOAT OBB::LenSegOnSeparateAxis(VECTOR Sep, VECTOR e1, VECTOR e2, VECTOR* e3)
{
    VECTOR _e3 = VGet(0, 0, 0);
    if (e3 != NULL) { _e3 = VGet(e3->x, e3->y, e3->z); };
    
    // 3つの内積の絶対値の和で投影線分長を計算
    // 分離軸Sepは標準化されていること
    float r1 = fabs(VDot(Sep, e1));
    FLOAT r2 = fabs(VDot(Sep, e2));
    FLOAT r3 = e3 ? (fabs(VDot(Sep, _e3))) : 0;
    return r1 + r2 + r3;
}

void OBB::CreateObb(VECTOR vpos, VECTOR size) {
    CreateObb(vpos, VGet(size.x, 0, 0), VGet(0, size.y, 0), VGet(0, 0, size.z));
}

void OBB::CreateObb(VECTOR vpos, VECTOR x, VECTOR y, VECTOR z) {

    m_Pos = vpos;
    m_Direct[0] = x;
    m_Direct[1] = y;
    m_Direct[2] = z;
    m_NormaDirect[0] = VNorm(x);
    m_NormaDirect[1] = VNorm(y);
    m_NormaDirect[2] = VNorm(z);
    m_fLength[0] = VSize(x);
    m_fLength[1] = VSize(y);
    m_fLength[2] = VSize(z);
    _hit = false;
    _existTime = -1;
    _effectiveTime = 0;
    _del = false;
}

void OBB::RotateObb(VECTOR rotate) {
    
    // MGetRotAxisを使用して回転行列取得
    MATRIX MatrixX = MGetRotAxis(VGet(1.0f, 0.0f, 0.0f),rotate.x);
    MATRIX MatrixY = MGetRotAxis(VGet(0.0f, 1.0f, 0.0f),rotate.y);
    MATRIX MatrixZ = MGetRotAxis(VGet(0.0f, 0.0f, 1.0f),rotate.z);

    for(auto i = 0; i < 3; ++i) {
        VECTOR v = m_Direct[i];

        v = VTransform(v, MatrixX);
        v = VTransform(v, MatrixY);
        v = VTransform(v, MatrixZ);

        m_Direct[i] = v;
        m_NormaDirect[i] = VNorm(v);
    }
    
}

void OBB::RotateObb(MATRIX matrix) {
    
    for(auto i = 0; i < 3; ++i) {
        VECTOR v = m_Direct[i];
        v = VTransform(v, matrix);
        m_Direct[i] = v;
        m_NormaDirect[i] = VNorm(v);
    }
}

void OBB::Update(VECTOR vpos) {
    m_Pos = vpos;
}

bool OBB::Update() {
    if (_effectiveTime >= 1) {
        --_effectiveTime;
        return false;
    }
    else if (_existTime >= 1) {
        --_existTime;
        if (_existTime < 1) {
            _del = true;
        }
    };
    return true;
}

void OBB::Update(VECTOR targetVPos, float spd) {
    if (Update()) {
        m_Pos = _trajectory(m_Pos, targetVPos, spd);
    }
}

void OBB::Update(CharacterBody& characterBody) {
    VECTOR CenterPos = VAdd(characterBody._bodyTop, characterBody._bodyUnder);
    m_Pos = VGet(CenterPos.x / 2, CenterPos.y / 2, CenterPos.z / 2);

    VECTOR bodyTilt = VSub(characterBody._bodyTop, characterBody._bodyUnder);
    bodyTilt = VNorm(bodyTilt);
    if(VSize(VSub(bodyTilt, characterBody._bodyTilt)) != 0) {
    MATRIX Matrix = MGetRotVec2(characterBody._bodyTilt, bodyTilt);

    RotateObb(Matrix);
    characterBody._bodyTilt = bodyTilt;
    }
}

void OBB::Render() {

    const float _r = 3.5f;
    const float _bigR = 5.0f;
    const int _sphereDivNum = 32;

    //直方体の点と線の色
    int _color, _color2;
    switch(_hit) {
    case true:
        _color = GetColor(255, 0, 0);
        _color2= GetColor(255, 0, 0);
        break;
    case false:
        _color = GetColor(0, 0, 255);
        _color2 = GetColor(0, 255, 0);
        break;
    }

    if (_effectiveTime) {
        _color = GetColor(0, 0, 0);
        _color2 = GetColor(0, 0, 0);
    }

    VECTOR c, a1, a2, a3, a4, b1, b2, b3, b4,
        _dX, _dY, _dZ, _reverseDX, _reverseDY, _reverseDZ,
        _positiveXFacePos, _negativeXFacePos, _positiveYFacePos,
        _negativeYFacePos, _positiveZFacePos, _negativeZFacePos;

    //方向ベクトル
    _dX = m_Direct[0];
    _dY = m_Direct[1];
    _dZ = m_Direct[2];

    //逆向きの方向ベクトル
    _reverseDX = Math::VMulti(_dX, -1);
    _reverseDY = Math::VMulti(_dY, -1);
    _reverseDZ = Math::VMulti(_dZ, -1);

    //obbの中心座標
    c = m_Pos;

    //中心から面への垂線と面、この交差点の座標(面の中心点)
    _positiveXFacePos = VAdd(c, _dX); _negativeXFacePos = VAdd(c, _reverseDX);
    _positiveYFacePos = VAdd(c, _dY); _negativeYFacePos = VAdd(c, _reverseDY);
    _positiveZFacePos = VAdd(c, _dZ); _negativeZFacePos = VAdd(c, _reverseDZ);

    //直方体の角の座標
    a1 = VAdd(_negativeZFacePos, VAdd(_reverseDX, _dY));
    a2 = VAdd(_negativeZFacePos, VAdd(_dX, _dY));
    a3 = VAdd(_negativeZFacePos, VAdd(_dX, _reverseDY));
    a4 = VAdd(_negativeZFacePos, VAdd(_reverseDX, _reverseDY));

    b1 = VAdd(_positiveZFacePos, VAdd(_reverseDX, _dY));
    b2 = VAdd(_positiveZFacePos, VAdd(_dX, _dY));
    b3 = VAdd(_positiveZFacePos, VAdd(_dX, _reverseDY));
    b4 = VAdd(_positiveZFacePos, VAdd(_reverseDX, _reverseDY));
    

    //OBBの中心座標を描画
    //DrawSphere3D(c, _bigR, _sphereDivNum, GetColor(255, 255, 0), _color, TRUE);

    //中心からの垂線と面の交差点の描画
    /*
    DrawSphere3D(_positiveXFacePos, _r, _sphereDivNum, _color, _color, TRUE);
    DrawSphere3D(_negativeXFacePos, _r, _sphereDivNum, _color, _color, TRUE);
    DrawSphere3D(_positiveYFacePos, _r, _sphereDivNum, _color, _color, TRUE);
    DrawSphere3D(_negativeYFacePos, _r, _sphereDivNum, _color, _color, TRUE);
    DrawSphere3D(_positiveZFacePos, _r, _sphereDivNum, _color, _color, TRUE);
    DrawSphere3D(_negativeZFacePos, _r, _sphereDivNum, _color, _color, TRUE);
    */
    //中心を通る、面から反対側の面への垂線を描画
    DrawLine3D(_positiveXFacePos, _negativeXFacePos, _color);
    DrawLine3D(_positiveYFacePos, _negativeYFacePos, _color);
    DrawLine3D(_positiveZFacePos, _negativeZFacePos, _color);

    //直方体の角を描画
    /*
    DrawSphere3D(a1, _r, _sphereDivNum, _color2, _color2, TRUE);
    DrawSphere3D(a2, _r, _sphereDivNum, _color2, _color2, TRUE);
    DrawSphere3D(a3, _r, _sphereDivNum, _color2, _color2, TRUE);
    DrawSphere3D(a4, _r, _sphereDivNum, _color2, _color2, TRUE);
                         
    DrawSphere3D(b1, _r, _sphereDivNum, _color2, _color, TRUE);
    DrawSphere3D(b2, _r, _sphereDivNum, _color2, _color, TRUE);
    DrawSphere3D(b3, _r, _sphereDivNum, _color2, _color, TRUE);
    DrawSphere3D(b4, _r, _sphereDivNum, _color2, _color, TRUE);
    */
    ////直方体の辺を描画
    DrawLine3D(a1, a2, _color2);
    DrawLine3D(a2, a3, _color2);
    DrawLine3D(a3, a4, _color2);
    DrawLine3D(a1, a4, _color2);
    
    DrawLine3D(b1, b2, _color2);
    DrawLine3D(b2, b3, _color2);
    DrawLine3D(b3, b4, _color2);
    DrawLine3D(b1, b4, _color2);
                       
    DrawLine3D(a1, b1, _color2);
    DrawLine3D(a2, b2, _color2);
    DrawLine3D(a3, b3, _color2);
    DrawLine3D(a4, b4, _color2);
    
}

