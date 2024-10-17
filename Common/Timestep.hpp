#pragma once

namespace zrn
{

class Timestep
{
public:
    Timestep(double time = 0.0f)
        : m_Time(time)
    { }

    operator double() const { return m_Time; }

    double Seconds() const { return m_Time; }

    double Milliseconds() const { return m_Time * 1.000; }

private:
    double m_Time;
};

} // namespace zrn