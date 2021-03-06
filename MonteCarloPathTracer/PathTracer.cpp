#include "PathTracer.h"
#include "iostream"
#include "vector"

using namespace std;

Vec3f PathTracer::BREFImportanceSample(Vec3f &direction, float Ns, REFLECT_TYPE type)
{
	float r1, r2;
	r1 = (float)rand() / RAND_MAX;
	r2 = (float)rand() / RAND_MAX;
	float phi = r1 * 2 * PI;
	float theta;

	switch (type)
	{
	case PathTracer::SPECULAR_REFLECT:
		theta = (float)acos(pow(r2, 1 / (Ns + 1)));
		break;
	case PathTracer::DIFFUSE_REFLECT:
		theta = (float)asin(sqrt(r2));
		break;
	default:
		break;
	}

	Vec3f sample(sin(theta)*cos(phi), cos(theta), sin(theta) * sin(phi));

	Vec3f front, right;
	if (fabs(direction.x) > fabs(direction.y))
	{
		front = normalize(Vec3f(direction.z, 0, -direction.x));
	}
	else
	{
		front = normalize(Vec3f(0, -direction.z, direction.y));
	}
	right = cross(direction, front);

	return normalize(sample.x * right + sample.y * direction + sample.z * front);
}

// 随机获取光线
Ray PathTracer::monteCarloSample(Ray &ray, Point3f &point, Material* &material, Vec3f &normal)
{
	Vec3f direction;

	float k1, k2;
	k1 = dot(material->Kd, Vec3f(1, 1, 1));
	k2 = dot(material->Ks, Vec3f(1, 1, 1)) + k1;

	if (k2 <= 0)
		return Ray(point, direction);

	// refraction
	if (material->Ni != 1.0)
	{
		float ni, nt;
		float cosTheta = dot(ray.direction, normal);

		Vec3f tNormal;
		if (cosTheta > 0.0)
		{
			ni = material->Ni;
			nt = 1.0f;
			tNormal = -normal;
		}
		else
		{
			ni = 1.0f;
			nt = material->Ni;
			tNormal = normal;
		}

		float f0 = (ni - nt) / (ni + nt);
		f0 *= f0;
		float fresnel = (float)(f0 + (1 - f0) * pow(1.0 - abs(cosTheta), 5));

		if (fresnel < (float)rand() / RAND_MAX)
		{
			if (ray.refract(tNormal, ni / nt, direction))
				return Ray(point, direction, Ray::SOURCE::TRANSMISSION);
			else if (ray.reflect(tNormal, direction))
			{
				return Ray(point, direction, Ray::SOURCE::SPECULAR_REFLECT);
			}
		}
	}

	if (k1 / k2 < (float)rand() / RAND_MAX)
	{
		Vec3f reflect;
		ray.reflect(normal, reflect);
		direction = BREFImportanceSample(reflect, material->Ns, REFLECT_TYPE::SPECULAR_REFLECT);
		return Ray(point, direction, Ray::SOURCE::SPECULAR_REFLECT);
	}
	else
	{
		direction = BREFImportanceSample(normal, material->Ns, REFLECT_TYPE::DIFFUSE_REFLECT);
		return Ray(point, direction, Ray::SOURCE::DIFFUSE_REFLECT);
	}
}

// 追踪，设置最大追踪深度
Color3f PathTracer::trace(Model &model, Ray &ray, int depth)
{
	Point3f point;
	Material* material;
	Vec3f normal;
	Color3f color, ambientIllumination, indirectIllumination, directIllumination;

	bool is_intersected = intersect(model, ray, point, material, normal);
	if (!is_intersected)
	{
		return Color3f(0, 0, 0);
	}
	else
	{
		if (depth > maxPathDepth)
			return material->Le;
		ambientIllumination = material->Ka * model.ambient;

		Ray nextRay = monteCarloSample(ray, point, material, normal);
		if (nextRay.source != Ray::SOURCE::NONE)
		{
			indirectIllumination = trace(model, nextRay, depth + 1);

			switch (nextRay.source)
			{
			case Ray::SOURCE::DIFFUSE_REFLECT:
				indirectIllumination *= material->Kd;
				break;
			case Ray::SOURCE::SPECULAR_REFLECT:
				indirectIllumination *= material->Ks;
				break;
			case Ray::SOURCE::TRANSMISSION:
				indirectIllumination *= material->Tf;
				break;
			default:
				break;
			}
		}
	}

	if (!(*model.lights).empty())
	{
		for (Light &light : *model.lights)
		{
			if (light.Le == Color3f(0.0f, 0.0f, 0.0f))
				continue;
			Color3f rgb;
			for (int i = 0; i < lightSampleNum; i++)
			{
				Point3f lightPoint = light.getRandomLightPoint();
				Vec3f lightDirection = lightPoint - point;
				float lightLength = length(lightDirection);

				Ray lightRay = Ray(point, lightDirection);
				lightRay.tmax = lightLength;

				is_intersected = intersect(model, lightRay, JudgeOrCal::JUDGE);
				if (!is_intersected)
				{
					lightDirection = normalize(lightDirection);
					float consIn = max(dot(normal, lightDirection), 0.0f);
					float consOut;
					switch (light.type)
					{
					case Light::TYPE::POLYGON:
					case Light::TYPE::TRIANGLE:
						consOut = max(dot(-lightDirection, light.normal), 0.0f);
						break;
					case Light::TYPE::SPHERE:
						consOut = max(dot(-lightDirection, -lightDirection), 0.0f);
						break;
					default:
						break;
					}
					float geoFactor = consIn * consOut / (lightLength * lightLength);

					const Vec3f &intensity = geoFactor * light.area * light.Le / (float)lightSampleNum;

					if (material->Kd != Vec3f(0, 0, 0))
					{
						float cosInOut = dot(lightDirection, normal);
						if (cosInOut > 0.0)
							rgb += cosInOut * material->Kd * intensity / PI;
					}

					if (material->Ks != Vec3f(0, 0, 0))
					{
						Vec3f v = ray.direction.flip();
						Vec3f h = normalize(lightDirection + v);
						float mDotH = dot(h, normal);
						if (mDotH > 0.0)
							rgb += pow(mDotH, material->Ns) * material->Ks * intensity * (material->Ns + 1) / (2 * PI);
					}
				}
			}
			directIllumination += rgb;
		}
	}

	color = material->Le + ambientIllumination + indirectIllumination + directIllumination;
	return color;
}

vector<float> PathTracer::render(Model& model)
{
	++iter_cnt;
	if (iter_cnt > maxRenderDepth)
	{
		return model.colors;
	}
	for (int y = 0; y < model.height; ++y)
	{
		for (int x = 0; x < model.width; ++x)
		{
			Color3f pxColor;
			vector<Ray> rays = model.getRays(x, y, pxSampleNum);
			for (Ray &ray : rays)
			{
				Color3f sampleColor = trace(model, ray).slip();
				pxColor += sampleColor;
			}
			pxColor /= (float)pxSampleNum;

			int index = (y * model.width + x) * 3;
			model.colors[index] = (model.colors[index] * (iter_cnt - 1) + pxColor.r) / iter_cnt;
			model.colors[index + 1] = (model.colors[index + 1] * (iter_cnt - 1) + pxColor.g) / iter_cnt;
			model.colors[index + 2] = (model.colors[index + 2] * (iter_cnt - 1) + pxColor.b) / iter_cnt;
		}
	}

	return model.colors;

	/* 最初为调试程序，临时写的获取一些像素颜色
	vector<float> colors = getColors(model.width, model.height);
	return colors;
	*/
}

// 临时获取像素颜色，最初测试用
vector<float> PathTracer::getColors(int w, int h)
{
	vector<float> colors;
	colors.resize(w*h * 3);

	int index;
	for (int i = 0; i < w; i++)
	{
		for (int j = 0; j < h; j++)
		{
			index = (j * w + i) * 3;
			if (i < 60)
			{
				colors[index] = 1;
				colors[index + 1] = 0;
				colors[index + 2] = 0;
			}
			else if (i < 120)
			{
				colors[index] = 0;
				colors[index + 1] = 1;
				colors[index + 2] = 0;
			}
			else
			{
				colors[index] = 0;
				colors[index + 1] = 0;
				colors[index + 2] = 1;
			}
		}
	}
	return colors;
}
