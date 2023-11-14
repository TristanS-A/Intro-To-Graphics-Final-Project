#pragma once
#include "../ew/ewMath/mat4.h"
#include "../ew/ewMath/vec3.h"
#include "../ew/ewMath/ewMath.h"

namespace tsa {

	//Identity matrix
	inline ew::Mat4 Identity() {
		return ew::Mat4(
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
		);
	};
	//Scale on x,y,z axes
	inline ew::Mat4 Scale(ew::Vec3 s) {
		return ew::Mat4(
			s.x, 0, 0, 0,
			0, s.y, 0, 0,
			0, 0, s.z, 0,
			0, 0, 0, 1
		);
	};
	//Rotation around X axis (pitch) in radians
	inline ew::Mat4 RotateX(float rad) {
		return ew::Mat4(
			1, 0, 0, 0,
			0, cos(rad), -sin(rad), 0,
			0, sin(rad), cos(rad), 0,
			0, 0, 0, 1
		);
	};
	//Rotation around Y axis (yaw) in radians
	inline ew::Mat4 RotateY(float rad) {
		return ew::Mat4(
			cos(rad), 0, sin(rad), 0,
			0, 1, 0, 0,
			-sin(rad), 0, cos(rad), 0,
			0, 0, 0, 1
		);
	};
	//Rotation around Z axis (roll) in radians
	inline ew::Mat4 RotateZ(float rad) {
		return ew::Mat4(
			cos(rad), -sin(rad), 0, 0,
			sin(rad), cos(rad), 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
		);
	};
	//Translate x,y,z
	inline ew::Mat4 Translate(ew::Vec3 t) {
		return ew::Mat4(
			1, 0, 0, t.x,
			0, 1, 0, t.y,
			0, 0, 1, t.z,
			0, 0, 0, 1
		);
	};

	struct Transform {
		ew::Vec3 position = ew::Vec3(0.0f, 0.0f, 0.0f);
		ew::Vec3 rotation = ew::Vec3(0.0f, 0.0f, 0.0f); //Euler angles (degrees)
		ew::Vec3 scale = ew::Vec3(1.0f, 1.0f, 1.0f);
		ew::Mat4 getModelMatrix() const {
			return tsa::Translate(position) * RotateY(ew::Radians(rotation.y)) * RotateX(ew::Radians(rotation.x)) * RotateZ(ew::Radians(rotation.z)) * tsa::Scale(scale);
		}
	};

    inline ew::Mat4 LookAt(ew::Vec3 eyePos, ew::Vec3 target, ew::Vec3 upVec){
        //Epic Gram-Schmidt moment

        ew::Vec3 forward = eyePos - target;
		forward = ew::Normalize(forward);

        ew::Vec3 right = ew::Cross(upVec, forward);
        right = ew::Normalize(right);

        ew::Vec3 up = ew::Cross(forward, right);
        up = ew::Normalize(up);

        return ew::Mat4(
                right.x, right.y, right.z, -ew::Dot(right, eyePos),
                up.x, up.y, up.z, -ew::Dot(up, eyePos),
                forward.x, forward.y, forward.z, -ew::Dot(forward, eyePos),
                0, 0, 0, 1
                );
    }

    inline ew::Mat4 Orthographic(float height, float aspect, float near, float far){
        float t = height / 2;
        float b = -t;
        float r = (height * aspect) / 2;
        float l = -r;
        return ew::Mat4(
                2 / (r - l), 0, 0, -(r + l) / (r - l),
                0, 2 / (t - b), 0, -(t + b) / (t - b),
                0, 0, -2 / (far - near), -(far + near) / (far - near),
                0, 0, 0, 1
                );
    }

    inline ew::Mat4 Perspective(float fov, float aspect, float near, float far){
        return  ew::Mat4(
                1 / (tan(fov / 2) * aspect), 0, 0, 0,
                0, 1 / (tan(fov / 2)), 0, 0,
                0, 0, (near + far) / (near - far), (2 * far * near) / (near - far),
                0, 0, -1, 0
                );
    }

	inline float clamp(float val, float low, float high) {
		if (val < low) {
			val = low;
		}
		else if (val > high) {
			val = high;
		}
	    return val;
	}
}
