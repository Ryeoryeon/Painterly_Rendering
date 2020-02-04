#include <vector>
#include <cmath>

//재완언니 코드

void makeVectorCoherent(float ori_dx, float ori_dy, float& coh_dx, float& coh_dy) 
{
	if ((ori_dx * coh_dx + ori_dy * coh_dy) < 0) {
		coh_dx = -coh_dx;
		coh_dy = -coh_dy;
	}
}

bool getFlowVectorInterpolated(int m_nWidth, int m_nHeight, float x, float y, float& dx, float& dy, const std::vector<std::vector<float>> & image_dx, const std::vector<std::vector<float>> & image_dy ) 
{
	if ((x < 0) || (x > (m_nWidth - 2)) || (y < 0) || (y > (m_nHeight - 2))) // ++x, ++y되는 것까지 생각 해 줘야 하니까 일단 -1 -> -2
		return false;

	int nx = x;
	int ny = y;

	if (x == (m_nWidth - 1)) {
		x -= 1.f;
		--nx;
	}
	if (y == (m_nHeight - 1)) {
		y -= 1.f;
		--ny;
	}

	float fx1 = x - nx;
	float fx2 = 1.f - fx1;
	float fy1 = y - ny;
	float fy2 = 1.f - fy1;

	int index = ny * m_nWidth * 2 + nx * 2; // 이거 일차 배열이어서 읽으려고 쓰신 것 같다.
	//일단 나는 이중벡터로 읽어왔으니까 수정..?
	float dx1 = image_dx[x][y];
	float dy1 = image_dy[x][y];
	float dx2 = image_dx[++x][++y];
	float dy2 = image_dy[x][y];

	index = (ny + 1) * m_nWidth * 2 + nx * 2;
	float dx3 = image_dx[x][y];
	float dy3 = image_dy[x][y];
	float dx4 = image_dx[++x][++y];
	float dy4 = image_dy[x][y];

	float fx2fy2 = fx2 * fy2;
	float fx1fy2 = fx1 * fy2;
	float fx2fy1 = fx2 * fy1;
	float fx1fy1 = fx1 * fy1;
	dx = (fx2fy2 * dx1) + (fx1fy2 * dx2) + (fx2fy1 * dx3) + (fx1fy1 * dx4);
	dy = (fx2fy2 * dy1) + (fx1fy2 * dy2) + (fx2fy1 * dy3) + (fx1fy1 * dy4);

	float fSize = sqrt(dx * dx + dy * dy);
	dx /= fSize;
	dy /= fSize;

	return true;
}

bool getFlowVectorRK4(int m_nWidth, int m_nHeight, float x, float y, float& dx, float& dy, const std::vector<std::vector<float>>& image_dx, const std::vector<std::vector<float>>& image_dy) 
{
	if ((x < 0) || (x > (m_nWidth - 1)) || (y < 0) || (y > (m_nHeight - 1)))
		return false;

	float asixth = 1.f / 6.f;
	float m_fStepLength = 1; // 어떤 값으로 해야 할 지 모르겠으니 일단 1로 설정하자.

	float dx1, dy1, dx2, dy2, dx3, dy3, dx4, dy4, dx_temp, dy_temp, ori_dx, ori_dy;

	ori_dx = dx_temp = dx;
	ori_dy = dy_temp = dy;
	dx1 = m_fStepLength * dx_temp;
	dy1 = m_fStepLength * dy_temp;

	getFlowVectorInterpolated(m_nWidth, m_nHeight, x + 0.5f * dx1, y + 0.5f * dy1, dx_temp, dy_temp, image_dx, image_dy);
	makeVectorCoherent(ori_dx, ori_dy, dx_temp, dy_temp);
	dx2 = m_fStepLength * dx_temp;
	dy2 = m_fStepLength * dy_temp;

	getFlowVectorInterpolated(m_nWidth, m_nHeight, x + 0.5f * dx2, y + 0.5f * dy2, dx_temp, dy_temp, image_dx, image_dy);
	makeVectorCoherent(ori_dx, ori_dy, dx_temp, dy_temp);
	dx3 = m_fStepLength * dx_temp;
	dy3 = m_fStepLength * dy_temp;

	getFlowVectorInterpolated(m_nWidth, m_nHeight, x + dx3, y + dy3, dx_temp, dy_temp, image_dx, image_dy);
	makeVectorCoherent(ori_dx, ori_dy, dx_temp, dy_temp);
	dx4 = m_fStepLength * dx_temp;
	dy4 = m_fStepLength * dy_temp;

	dx = (dx1 + 2 * dx2 + 2 * dx3 + dx4) * asixth;
	dy = (dy1 + 2 * dy2 + 2 * dy3 + dy4) * asixth;

	return true;
}
