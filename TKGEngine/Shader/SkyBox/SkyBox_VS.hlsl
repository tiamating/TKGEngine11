#include "SkyBox.hlsli"

PS_IN main(uint idx : SV_VertexID)
{
	PS_IN vout;
	
	// ���_����
	vout.sv_pos = float4(((idx >> 0) & 0x01) * 2.0 - 1.0, ((idx >> 1) & 0x01) * 2.0 - 1.0, 0.0, 1.0);
	// �t���X�^���̃��[���h��Ԃł̃G�b�W�����x�N�g��
	float4 view_edge_dir = float4(normalize(mul(camera.IP, vout.sv_pos).xyz), 0.0);
	// view�s��̉�]�����ŃG�b�W�����݂̃J���������Ɍ�����
	matrix view_rot = camera.IV;
	view_rot._41_42_43 = 0.0;
	vout.wpos = mul(view_rot, view_edge_dir);
	
	return vout;
}