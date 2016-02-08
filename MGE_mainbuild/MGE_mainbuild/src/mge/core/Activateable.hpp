#ifndef ACTIVATEABLE_H
#define ACTIVATEABLE_H

class Activateable
{
public:
    virtual void SetActive(bool state);
    bool IsActive();
protected:
    bool _isActive = true;
    ~Activateable();
};


#endif // ACTIVEATEABLE_H
