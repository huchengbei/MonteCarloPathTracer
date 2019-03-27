# Monte Carlo Path Tracer
CG��ҵ ���ؿ���·��׷��

## ƽ̨����
**����ϵͳ��** Win10 x64

**IDE��** VS2017

**������**
- OpenCV (���ڻ�ͼ)
- yaml-cpp (���ڶ�ȡģ�͵�һЩ����)

## ʹ�÷���
�������������� "programname.exe model.yaml"

�磺

```
MontoCarloPathTracer example.yaml
```
��ֱ�Ӱ������ļ��϶��������ļ��ϡ�(��֪Ϊ�Σ���ʹ���϶����������ļ��ͳ����ļ�����ͬһĿ¼���ʽ���������ʹ��)

�ڽ�������е���ʱ����Ҫ����Ŀ�����У����ԵĲ�������Ϊ�����ļ���ַ

## ����
1. ��ȡobj�ļ��Լ�mtl�ļ���Ⱦ����
1. ʵʱ��ʾ��ǰ��Ⱦ���
1. ���и߹ⷴ�䡢�����������Ч��
1. ����ʹ�ó����ڲ���Դ������modelʱ�Զ���ȡ����Ҳ����������ӹ�Դ���������ʾ��Ĭ�Ͻ�ʹ���ⲿ��ӹ�Դ��
1. ��ʵʱ����������ĳ���ͼ
1. �����л���ʾģ���ļ�����Ƭ�����ָ���������Ƭ������Ⱦ������С���Լ�ÿ�ε�����ʱ�䣬��ʱ�䣬ƽ��ʱ��

## �ص�
1. ʹ�������ļ�����ģ�ͣ�����ÿ�θĴ��룬ֻ��������ļ�����
1. ʹ��KDTree����Ƭ�������м���
1. �Թ�Դ���з��࣬��ͬ��Դ��ͬ����
1. ���õ���������ε�����ÿ������ֻ����һ�����ߣ����ӵ㸽���������ҽ��ж�εĵ�������֮ǰ�ĵ���Ч������ƽ�����൱��ÿ�����ؽ����˶�β���������ÿ�ε��������Կ�����ǰ����Ч��


## ע��
����Ŀ������һЩ�ļ��������л����¿���ֱ�ӵ������У�����Ҫ�������Ŀ����

����Ŀ��VS���޷����ɣ������һ���Ŀ���ض�����Ŀ

��ȷ����������path�����opencv_world400d.dll��opencv_world400.dll����Ŀ¼�������������ļ������ͬĿ¼�������޷�����

## ��Ŀ��Ӧ���ݽṹ
- Box: ��Χ�У�����һ���������Χ�����е�Ԫ�أ��޶�����Ԫ�صķ�Χ
- MetaClass�� ����һЩ���࣬�����������㣬��ɫ���Լ�һЩ�����������
- Material�� �����࣬�ڼ���ģ��ʱԤ�ȼ��ز��ʣ��������������е���Ӧ����
- Triangle�� ������Ƭ������ģ��ʱ�Ὣ������Ƭ�ָ�Ϊ������Ƭ�������������
- Face�� ��Ƭ�࣬��KDTree��������Ӧ��Ƭʱ���ٴ�����ȡ��Ӧ�����μ��㽻��
- Model�� ģ���࣬�����ǳ����࣬���峡���е�Ԫ��
- Light�� ��Դ�࣬������λ�ã���С������ȣ�ֱ�ӹ���ʹ��
- Ray�� �����࣬Ҳ�������࣬�������ߣ����ߣ����ͷ����Լ���ǰ��󳤶�
- KDTree�� KD���࣬���ڶ�ģ���е���Ƭ�������������������Ƭ����
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

## ��Ⱦ���ͼ
|Name|iter-1|iter-50|iter-100|
|---|---|---|---|
|Cup|![](pics/cup/0001.jpg)|![](pics/cup/0050.jpg)|![](pics/cup/0100.jpg)|
|Room|![](pics/room/0001.jpg)|![](pics/room/0050.jpg)|![](pics/room/0100.jpg)|
|VeachMIS|![](pics/VeachMIS/0001.jpg)|![](pics/VeachMIS/0050.jpg)|![](pics/VeachMIS/0100.jpg)|
|Classical|![](pics/classical/0001.jpg)|![](pics/classical/0050.jpg)|![](pics/classical/0100.jpg)|
|Vase|![](pics/vase/0001.jpg)|![](pics/vase/0050.jpg)|![](pics/vase/0100.jpg)|