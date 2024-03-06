#include <iostream>

#include "ParticleBuoyantForce.h"

const float ParticleBuoyantForce::GravityAccelY = -9.8f;

ParticleBuoyantForce::ParticleBuoyantForce(float liquidSurfaceY, float particleSize, float liquidDensity, float hydroDragCoeff) {
    this->liquidSurfaceY = liquidSurfaceY;
    this->particleSize = particleSize;
    this->liquidDensity = liquidDensity;
    this->hydroDragCoeff = hydroDragCoeff;

    particleHalfSize = particleSize / 2.0f;
    particleVolume = particleSize * particleSize * particleSize;
}

// Размер частицы фиксирован в текущем генераторе силы. Следовательно, фиксирован и объем частицы.
// Поэтому, варьируя массу входной частицы, мы тем самым варьируем плотность материала частицы.
// NOTE: Все рассмотрения происходят в проекции на ось Y.
void ParticleBuoyantForce::ApplyTo(Particle* particle) {
    // Координата Y нижней части частицы.
    float particleBottomY = particle->pos.y - particleHalfSize;

    // Высота погружения - это расстояние от нижней части частицы до поверхности воды.
    float submersionHeight = liquidSurfaceY - particleBottomY;

    // Частица не погружена в жидкость.
    if (submersionHeight <= 0.0f)
        return;

    // Когда частица полностью погружена в жидкость, объем вытесненной жидкости равен полному объему частицы.
    float displacedLiquidVolume = particleVolume;

    // Когда частица погружена не полностью, объем вытесненной жидкости равен объему погруженной части.
    // 
    // NOTE: В нашей модели полный объем частицы распределен равномерно вдоль её высоты (формально - объем пропорционален высоте).
    // Это означает, что, например, если взять N процентов от полной высоты, то объем, соответствующий этой высоте,
    // будет составлять от полного объема те же N процентов.
    // 
    // Следовательно, зная процент погруженной высоты, мы знаем также процент погруженного объема.
    if (submersionHeight < particleSize)
        displacedLiquidVolume = particleVolume * (submersionHeight / particleSize);

    // По определению, выталкивающая сила по модулю равна весу вытесненной жидкости и направлена вверх.
    float displacedLiquidWeightY = (displacedLiquidVolume * liquidDensity) * GravityAccelY;
    Vec2 buoyantForce(0.0f, -displacedLiquidWeightY);
    particle->ApplyForce(buoyantForce);
    
    std::cout << "buoyantForce.y: " << buoyantForce.y << std::endl;

    Vec2 hydroDragForce = -particle->velocity * hydroDragCoeff;
    particle->ApplyForce(hydroDragForce);

    std::cout << "hydroDragForce.y: " << hydroDragForce.y << std::endl;
}