#include "dwLIC2.h"
#include <opencv2/opencv.hpp>
//재완언니가 준 코드

void dwLIC2::setFlowField(int x, int y, float dx, float dy) {
	if (x < 0 || x >(m_nWidth - 1) || y < 0 || y >(m_nHeight - 1))
	{
		std::cerr << "index error in setFlowField" << std::endl;
		return;
	}
	int index = m_nWidth * 2 * y + 2 * x;
	m_pFlowField[index] = dx;
	m_pFlowField[++index] = dy;
}

void dwLIC2::setNoiseField(int x, int y) {
	if (x < 0 || x >(m_nWidth - 1) || y < 0 || y >(m_nHeight - 1))
	{
		std::cerr << "index error in setNoiseField" << std::endl;
		return;
	}
	int nx = x + 0.5f;
	int ny = y + 0.5f;
	if (m_pIMG) {
		int index = m_pIMG->widthStep * ny + nx * m_pIMG->nChannels;
		m_pNoiseField[index] = m_pIMG->imageData[index];
		m_pNoiseField[index + 1] = m_pIMG->imageData[index + 1];
		m_pNoiseField[index + 2] = m_pIMG->imageData[index + 2];
	}
	else {
		int index = m_nWidth * ny + nx;
		m_pNoiseField[index] = (unsigned char)(rand() % 256);
	}
	m_noiseList.push_back(dwNoise(x, y));

}

void dwLIC2::doLICForward() {

	for (auto& noise : m_noiseList) {
		m_pCurve[0].clear();
		m_pCurve[1].clear();

		float x, y;
		float ori_x = x = noise.x;
		float ori_y = y = noise.y;
		float ori_dx, ori_dy;

		int sum = 0;
		int count = 0;
		for (int i = 0; i < 2; ++i)
			m_pCurve[i].push_back(dwNoise(noise.x, noise.y));

		int dir = 0;
		int idx = 2;
		for (int i = -1; i < 2; i += 2, ++dir) {
			float dx, dy;
			x = ori_x;
			y = ori_y;
			if (!getFlowVectorInterpolated(x, y, ori_dx, ori_dy))
				continue;
			dx = (ori_dx * i);
			dy = (ori_dy * i);
			if (!getFlowVectorRK4(x, y, dx, dy))
				continue;

			for (int j = m_nLength; j > 0; --j)
			{
				if (((x + dx) > (m_nWidth - 1)) || ((x + dx) < 0) || ((y + dy) > (m_nHeight - 1)) || ((y + dy) < 0))
					break;
				else {
					x += dx;
					y += dy;

					m_pCurve[dir].push_back(dwNoise(x, y));

					if (!getFlowVectorRK4(x, y, dx, dy))
						break;
				}
			}
		}

		float tot = 0.f;
		float totB = 0.f;
		float totG = 0.f;
		float totR = 0.f;
		float tot2 = 0.f;

		for (int i = 0; i < 2; i++) {
			for (int j = 0; j < m_pCurve[i].size(); j++) {
				if (i == 0 && j == 0)
					continue;
				int x = (int)(m_pCurve[i][j].x + 0.5f);
				int y = (int)(m_pCurve[i][j].y + 0.5f);
				float weight = m_pGaussianWeight[j];

				if (m_pIMG) {
					totB += (m_pNoiseField[y * m_pIMG->widthStep + x * m_pIMG->nChannels + 0] * weight);
					totG += (m_pNoiseField[y * m_pIMG->widthStep + x * m_pIMG->nChannels + 1] * weight);
					totR += (m_pNoiseField[y * m_pIMG->widthStep + x * m_pIMG->nChannels + 2] * weight);
				}
				else
					tot += (m_pNoiseField[y * m_nWidth + x] * weight);
				tot2 += weight;
			}
		}
		if (m_pIMG) {
			if (tot2 != 0.0) {
				totB /= tot2;
				totG /= tot2;
				totR /= tot2;
			}
			m_pLICField[int(noise.y + 0.5f) * m_pIMG->widthStep + int(noise.x + 0.5f) * m_pIMG->nChannels + 0] = (unsigned char)totB;
			m_pLICField[int(noise.y + 0.5f) * m_pIMG->widthStep + int(noise.x + 0.5f) * m_pIMG->nChannels + 1] = (unsigned char)totG;
			m_pLICField[int(noise.y + 0.5f) * m_pIMG->widthStep + int(noise.x + 0.5f) * m_pIMG->nChannels + 2] = (unsigned char)totR;
		}
		else {
			if (tot2 != 0.0)
				tot /= tot2;
			m_pLICField[int(noise.y + 0.5f) * m_nWidth + int(noise.x + 0.5f)] = (unsigned char)tot;
		}
	}
}

bool dwLIC2::getFlowVectorRK4(float x, float y, float& dx, float& dy) {
	if ((x < 0) || (x > (m_nWidth - 1)) || (y < 0) || (y > (m_nHeight - 1)))
		return false;

	float dx1, dy1, dx2, dy2, dx3, dy3, dx4, dy4, dx_temp, dy_temp, ori_dx, ori_dy;

	ori_dx = dx_temp = dx;
	ori_dy = dy_temp = dy;
	dx1 = m_fStepLength * dx_temp;
	dy1 = m_fStepLength * dy_temp;

	getFlowVectorInterpolated(x + 0.5f * dx1, y + 0.5f * dy1, dx_temp, dy_temp);
	makeVectorCoherent(ori_dx, ori_dy, dx_temp, dy_temp);
	dx2 = m_fStepLength * dx_temp;
	dy2 = m_fStepLength * dy_temp;

	getFlowVectorInterpolated(x + 0.5f * dx2, y + 0.5f * dy2, dx_temp, dy_temp);
	makeVectorCoherent(ori_dx, ori_dy, dx_temp, dy_temp);
	dx3 = m_fStepLength * dx_temp;
	dy3 = m_fStepLength * dy_temp;

	getFlowVectorInterpolated(x + dx3, y + dy3, dx_temp, dy_temp);
	makeVectorCoherent(ori_dx, ori_dy, dx_temp, dy_temp);
	dx4 = m_fStepLength * dx_temp;
	dy4 = m_fStepLength * dy_temp;

	dx = (dx1 + 2 * dx2 + 2 * dx3 + dx4) * asixth;
	dy = (dy1 + 2 * dy2 + 2 * dy3 + dy4) * asixth;

	return true;
}

bool dwLIC2::getFlowVectorInterpolated(float x, float y, float& dx, float& dy) {
	if ((x < 0) || (x > (m_nWidth - 1)) || (y < 0) || (y > (m_nHeight - 1)))
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

	int index = ny * m_nWidth * 2 + nx * 2;
	float dx1 = m_pFlowField[index];
	float dy1 = m_pFlowField[++index];
	float dx2 = m_pFlowField[++index];
	float dy2 = m_pFlowField[++index];

	index = (ny + 1) * m_nWidth * 2 + nx * 2;
	float dx3 = m_pFlowField[index];
	float dy3 = m_pFlowField[++index];
	float dx4 = m_pFlowField[++index];
	float dy4 = m_pFlowField[++index];

	float fx2fy2 = fx2 * fy2;
	float fx1fy2 = fx1 * fy2;
	float fx2fy1 = fx2 * fy1;
	float fx1fy1 = fx1 * fy1;
	dx = (fx2fy2 * dx1) + (fx1fy2 * dx2) + (fx2fy1 * dx3) + (fx1fy1 * dx4);
	dy = (fx2fy2 * dy1) + (fx1fy2 * dy2) + (fx2fy1 * dy3) + (fx1fy1 * dy4);

	float fSize = std::sqrt(dx * dx + dy * dy);
	dx /= fSize;
	dy /= fSize;

	return true;
}