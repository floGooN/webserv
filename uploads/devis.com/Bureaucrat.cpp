#include "Bureaucrat.hpp"

Bureaucrat::Bureaucrat() :name("default bureaucrat")
{
	this->grade = 75;
}

Bureaucrat::Bureaucrat(std::string name, int nb) : name(name)
{
	if (nb < 1)
		throw GradeTooHighException();
	else if (nb > 150)
		throw GradeTooLowException();
	else
		this->grade = nb;
}


Bureaucrat::Bureaucrat(Bureaucrat const &other)
{
	*this = other;
}

Bureaucrat &Bureaucrat::operator=(const Bureaucrat &other)
{
	if (this != &other)
	{
		this->grade = other.grade;
	}
	return *this;
}

Bureaucrat::~Bureaucrat() {
}

std::string Bureaucrat::getName(void) const
{
	return this->name;
}

int Bureaucrat::getGrade(void) const
{
	return this->grade;
}

void Bureaucrat::incrementGrade(void)
{
	if (this->grade - 1 < 1)
		throw GradeTooHighException();
	else
		this->grade -= 1;
}
void Bureaucrat::decrementGrade(void)
{
	if (this->grade + 1 > 150)
		throw GradeTooLowException();
	else
		this->grade += 1;
}

const char*	Bureaucrat::GradeTooLowException::what() const throw() 
{
	return "Bureaucrat grade too low!";
}

const char*	Bureaucrat::GradeTooHighException::what() const throw() 
{
	return "Bureaucrat grade too high!";
}

std::ostream &operator<<(std::ostream &os, const Bureaucrat &bureau)
{
    os << bureau.getName() << ", bureaucrat grade " << bureau.getGrade(); 
    return os; 
}
