/**
* LCTypesEx.cpp
* 24.08.2023
* (c) Denis Romakhov
*/

#include "LCTypesEx.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_projection.hpp"
#include "glm/ext/matrix_transform.hpp"


LcVector2 LcDefaults::OneVec2 = LcVector2{ 1.0f, 1.0f };
LcVector2 LcDefaults::ZeroVec2 = LcVector2{ 0.0f, 0.0f };
LcVector3 LcDefaults::OneVec3 = LcVector3{ 1.0f, 1.0f, 1.0f };
LcVector3 LcDefaults::ZeroVec3 = LcVector3{ 0.0f, 0.0f, 0.0f };
LcVector4 LcDefaults::OneVec4 = LcVector4{ 1.0f, 1.0f, 1.0f, 1.0f };
LcVector4 LcDefaults::ZeroVec4 = LcVector4{ 0.0f, 0.0f, 0.0f, 0.0f };
LcColor4 LcDefaults::Invisible = LcColor4{ 0.0f, 0.0f, 0.0f, 0.0f };
LcColor4 LcDefaults::White4 = LcColor4{ 1.0f, 1.0f, 1.0f, 1.0f };
LcColor3 LcDefaults::White3 = LcColor3{ 1.0f, 1.0f, 1.0f };
LcColor4 LcDefaults::Black4 = LcColor4{ 0.0f, 0.0f, 0.0f, 1.0f };
LcColor3 LcDefaults::Black3 = LcColor3{ 0.0f, 0.0f, 0.0f };
LcSizef LcDefaults::ZeroSize = LcSizef{ 0.0f, 0.0f };

glm::vec4 LcDefaults::OneXVec4 = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
glm::vec4 LcDefaults::ZeroXVec4 = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);


LcRectf ToF(const LcRect& rect)
{
	return LcRectf {
		static_cast<float>(rect.left),
		static_cast<float>(rect.top),
		static_cast<float>(rect.right),
		static_cast<float>(rect.bottom)
	};
}

LcRectf ToF(const LcVector2& leftTop, const LcVector2& rightBottom)
{
	return LcRectf{ leftTop.x, leftTop.y, rightBottom.x, rightBottom.y };
}

LcRect ToI(const LcRectf& rect)
{
	return LcRect {
		static_cast<int>(rect.left),
		static_cast<int>(rect.top),
		static_cast<int>(rect.right),
		static_cast<int>(rect.bottom)
	};
}

LcMatrix4 OrthoMatrix(float widthPixels, float heightPixels, float nearPlane, float farPlane, bool flipY, bool transpose)
{
	auto matrix = glm::orthoLH_ZO(
		0.0f, widthPixels,
		flipY ? heightPixels : 0.0f,
		flipY ? 0.0f : heightPixels,
		nearPlane, farPlane
	);

	matrix[3][0] = 0.0f;
	matrix[3][1] = 0.0f;

	return transpose ? glm::transpose(matrix) : matrix;
}

LcMatrix4 OrthoMatrix(LcSize vp, float nearPlane, float farPlane, bool flipY, bool transpose)
{
	return OrthoMatrix((float)vp.x, (float)vp.y, nearPlane, farPlane, flipY, transpose);
}

LcMatrix4 LookAtMatrix(LcVector3 from, LcVector3 to, bool transpose)
{
	auto matrix = glm::lookAtLH(
		glm::vec3(from.x, from.y, from.z),
		glm::vec3(to.x, to.y, to.z),
		glm::vec3(0.0f, 1.0f, 0.0f)
	);
	return transpose ? glm::transpose(matrix) : matrix;
}

LcMatrix4 TranslationMatrix(LcVector3 pos)
{
	return glm::translate(glm::mat4(1.0f), glm::vec3(pos.x, pos.y, pos.z));
}

LcMatrix4 TransformMatrix(LcVector3 pos, LcVector2 scale, float rotZ, bool flipY, bool transpose)
{
	constexpr static float d2r = 180.0f / LcPI;

	auto matrix = glm::translate(glm::mat4(1.0f), glm::vec3(pos.x, pos.y, pos.z));
	matrix = glm::rotate(matrix, glm::radians(rotZ * d2r), glm::vec3(0.0, 0.0, 1.0));
	matrix = glm::scale(matrix, glm::vec3(scale.x, flipY ? -scale.y : scale.y, 1.0f));

	return transpose ? glm::transpose(matrix) : matrix;
}

LcMatrix4 TransposeMatrix(const LcMatrix4& mat)
{
	return glm::transpose(mat);
}

LcMatrix4 IdentityMatrix()
{
	return glm::identity<glm::mat4x4>();
}
