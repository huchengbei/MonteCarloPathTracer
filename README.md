# Monte Carlo Path Tracer
CG��ҵ ���ؿ���·��׷��

## ƽ̨����
**����ϵͳ��** Win10 x64

**IDE��** VS2017

**������** OpenCV (���ڻ�ͼ)

## ����
1. ��ȡobj�ļ��Լ�mtl�ļ���Ⱦ����
1. ʵʱ��ʾ��ǰ��Ⱦ���
1. ���и߹ⷴ�䡢�����������Ч��
1. ����ʹ�ó����ڲ���Դ������modelʱ�Զ���ȡ����Ҳ�����������ӹ�Դ���������ʾ��Ĭ�Ͻ�ʹ���ⲿ���ӹ�Դ��
1. ��ʵʱ����������ĳ���ͼ
1. �����л���ʾģ���ļ�����Ƭ�����ָ���������Ƭ������Ⱦ������С���Լ�ÿ�ε�����ʱ�䣬��ʱ�䣬ƽ��ʱ��

## �ص�
1. ʹ��KDTree����
1. ���õ���������ε�����ÿ������ֻ����һ�����ߣ����ӵ㸽���������ҽ��ж�εĵ�������֮ǰ�ĵ���Ч������ƽ�����൱��ÿ�����ؽ����˶�β���������ÿ�ε��������Կ�����ǰ����Ч��


## ʹ�÷���
ֱ�Ӵ򿪽�������ļ������м��ɣ�Ĭ�ϼ���scene0��ģ���ļ���������main������ѡ��Ҫ����ģ�͡�

������exe�ļ�ʱ������Ӧexe�ļ����ϼ�Ŀ¼������models�ļ��м���Ӧģ���ļ���Ҳ��ֱ��ʹ�á�

## ע��
����Ŀ������һЩ�ļ��������л����¿���ֱ�ӵ������У�����Ҫ�������Ŀ����

����Ŀ��VS���޷����ɣ������һ���Ŀ���ض�����Ŀ

��ȷ����������path�����opencv_world400d.dll��opencv_world400.dll����Ŀ¼�������������ļ������ͬĿ¼�������޷�����

## ��Ŀ��Ӧ���ݽṹ
- Box: ��Χ�У�����һ���������Χ�����е�Ԫ�أ��޶�����Ԫ�صķ�Χ
- MetaClass�� ����һЩ���࣬�����������㣬��ɫ���Լ�һЩ�����������
- Material�� �����࣬�ڼ���ģ��ʱԤ�ȼ��ز��ʣ��������������е���Ӧ����
- Triangle�� ������Ƭ������ģ��ʱ�Ὣ������Ƭ�ָ�Ϊ������Ƭ�������������
- Model�� ģ���࣬�����ǳ����࣬���峡���е�Ԫ��
- Light�� ��Դ�࣬������λ�ã���С������ȣ�ֱ�ӹ���ʹ��
- Ray�� �����࣬Ҳ�������࣬�������ߣ����ߣ����ͷ����Լ���ǰ��󳤶�
- KDTree�� KD���࣬���ڶ�ģ���е�Ԫ�ؽ������������������Ƭ����
- Camera�� ����࣬�����ӵ��Լ��ӵ㷽���൱�ڱ�׼���۾�����λ�á����򡢽Ƕ�
- PathTracer�� ·��׷���࣬�Ը�����ģ�ͽ��й���׷�٣������Ⱦ���
- --
- ���⻹�и�functions�����ļ������ڶ����������ģ��ʹ�õ��ĺ�������Ҫʱ�����󽻺���
- Main�ļ���Ҫ���ڳ����������չʾ

## �㷨����
```
for each pixel
	rays = model.getRays(pixel)
	for ray in rays
		sampleColor = trace(ray)
		pixelColor += sampleColor
	end for
	pixelColor /= the size of rays
	
	// for each iterate, the weight of model.color and pixel Color is different
	// newColor = (oldColor * (iter_cnt - 1) + pixelColor) / iter_cnt
	model.color = the mean of model.color and pixelColor
end for
```

```
Color trace(ray)
{
	if the depth of trace > maxDepth
		return BLACK
	color = NULL
	ambientIllumination = Ka of intersection * ambient
	indirectIllumination = NULL
	directIllumination = NULL

	newRay = monteCarloSample(ray)
	indirectIllumination = trace(newRay) * (Kd, Ks or Tf of intersection)

	for each light
		directIllumination += the color from this light
	end for
	
	color  = Ke of intersection + ambientIllumination + indirectIllumination + directIllumination 

	return color
}
```