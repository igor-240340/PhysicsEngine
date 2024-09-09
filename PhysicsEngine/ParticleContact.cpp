#include "ParticleContact.h"

void ParticleContact::resolve() {
    resolve_velocity();
    resolve_interpenetration();
}

void ParticleContact::resolve_velocity() {
    float normal_velocity = calculate_normal_velocity();

    // Если масса A движется в направлении нормали удара или перпендикулярно ей, то удара нет.
    // Такая ситуация возможна в т.ч. сразу после резолва.
    if (normal_velocity <= 0.0f)
        return;

    // Ударные импульсы возникают вдоль нормали удара, имеют равные модули и противоположные направления.
    // 
    // Почему импульс выражен через обратные массы, а не прямые?
    // Если обратная масса и скорость второй точки равны нулю, то
    // данная формула без изменений будет эквивалентна формуле
    // для вычисления ударного импульса при столкновении точки с неподвижным объектом с бесконечно большой массой.
    // Поэтому, если мы хотим смоделировать удар о неподвижную стену, достаточно представить стену точкой с нулевой обратной массой.
    float inv_mass_a = particle_a->invMass;
    float inv_mass_b = particle_b->invMass;
    float impulse_abs = normal_velocity * (1 + restitution) / (inv_mass_a + inv_mass_b);
    Vec3 impulse_a = hit_normal * impulse_abs;
    Vec3 impulse_b = -hit_normal * impulse_abs;

    particle_a->velocity += impulse_a * particle_a->invMass;
    particle_b->velocity += impulse_b * particle_b->invMass;
}

void ParticleContact::resolve_interpenetration() {
    float inv_mass_a = particle_a->invMass;
    float inv_mass_b = particle_b->invMass;

    // Мы делим полную величину проникновения между двумя точками по такому принципу:
    // во сколько раз первая масса больше второй, во столько раз смещение первой массы будет меньше смещения второй.
    // Другими словами, отношение смещений точек будет равно обратному отношению их масс.
    //
    // В чем преимущество выражения смещения через обратные массы?
    // Если обратная масса второй точки будет равна нулю,
    // то первая точка будет сдвинута на полную величину проникновения, а вторая точка не изменит положения.
    //
    // Следовательно, если вторая точка представляет, например, неподвижную стену,
    // то в устранении проникновения она участвовать не будет.
    float displacement_a = penetration * inv_mass_a / (inv_mass_a + inv_mass_b);
    float displacement_b = penetration - displacement_a;

    particle_a->pos += hit_normal * displacement_a;
    particle_b->pos += -hit_normal * displacement_b;
}

float ParticleContact::calculate_normal_velocity() {
    // Скорость A в предположении, что B неподвижно - относительная скорость.
    Vec3 velocity_a_rel_b = particle_a->velocity - particle_b->velocity;

    // Нормальная составляющая относительной скорости A в СК удара, где
    // СК удара - это СК с осями нормаль/касательная,
    // причем ось нормали направлена обратно нормали удара,
    // что даёт положительную проекцию относительной скорости A на нормаль, когда A движется против нормали удара.
    Vec3 normal_axis = -hit_normal;

    return Vec3::dot(velocity_a_rel_b, normal_axis);
}
