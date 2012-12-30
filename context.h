#ifndef _CONTEXT_H_
#define _CONTEXT_H_

#include <iterator>
#include <map>
#include <string>
#include <vector>
#include "mstwpdf.h"
#include "dss_pinlo.h"
#include "coupling.h"
#include "gluondist.h"

class Context {
public:
    double x0;
    double A;
    double c;
    double lambda;
    double mu2;
    double Nc;
    double Nf;
    double CF;
    double TR;
    double Sperp;
    double pT2;
    double sqs;
    double Y;
    std::string pdf_filename;
    std::string ff_filename;

    GluonDistribution* gdist;
    Coupling* cpl;

    double Q02x0lambda;
    double tau;

    Context(
        double x0,
        double A,
        double c,
        double lambda,
        double mu2,
        double Nc,
        double Nf,
        double CF,
        double TR,
        double Sperp,
        double pT2,
        double sqs,
        double Y,
        std::string pdf_filename,
        std::string ff_filename,
        GluonDistribution* gdist,
        Coupling* cpl) :
     x0(x0),
     A(A),
     c(c),
     lambda(lambda),
     mu2(mu2),
     Nc(Nc),
     Nf(Nf),
     CF(CF),
     TR(TR),
     Sperp(Sperp),
     pT2(pT2),
     sqs(sqs),
     Y(Y),
     pdf_filename(pdf_filename),
     ff_filename(ff_filename),
     gdist(gdist),
     cpl(cpl),
     Q02x0lambda(c * pow(A, 1.0d/3.0d) * pow(x0, lambda)),
     tau(sqrt(pT2)/sqs*exp(Y)) {}
    Context(const Context& other) :
     x0(other.x0),
     A(other.A),
     c(other.c),
     lambda(other.lambda),
     mu2(other.mu2),
     Nc(other.Nc),
     Nf(other.Nf),
     CF(other.CF),
     TR(other.TR),
     Sperp(other.Sperp),
     pT2(other.pT2),
     sqs(other.sqs),
     Y(other.Y),
     pdf_filename(other.pdf_filename),
     ff_filename(other.ff_filename),
     gdist(other.gdist),
     cpl(other.cpl),
     Q02x0lambda(other.Q02x0lambda),
     tau(other.tau) {}
};

std::ostream operator<<(std::ostream out, Context& ctx);

class ContextCollection {
public:
    typedef std::vector<Context>::iterator iterator;

    ContextCollection() :
      gdist(NULL),
      cpl(NULL) {
        setup_defaults();
    }
    ContextCollection(const std::string& filename) :
      gdist(NULL),
      cpl(NULL) {
        setup_defaults();
        ifstream in(filename.c_str());
        read_config(in);
        in.close();
    }
    ~ContextCollection() {
        delete gdist;
        gdist = NULL;
        delete cpl;
        cpl = NULL;
    }
    Context& get_context(size_t n);
    Context& operator[](size_t n);
    bool empty();
    size_t size();
    iterator begin();
    iterator end();
    
    void erase(std::string key);
    void set(std::string key, std::string value);
    void add(std::string key, std::string value);
    
    GluonDistribution* gdist;
    Coupling* cpl;

    friend std::istream& operator>>(std::istream& in, ContextCollection& cc);
    friend std::ostream& operator<<(std::ostream& out, ContextCollection& cc);
    friend class ThreadLocalContext;
    
private:
    std::multimap<std::string, std::string> options;
    std::vector<Context> contexts;
    void setup_defaults();
    void read_config(std::istream& in);
    void create_contexts();
};

std::istream& operator>>(std::istream& in, ContextCollection& cc);
std::ostream& operator<<(std::ostream& out, ContextCollection& cc);

class ThreadLocalContext {
    friend class IntegrationContext;
    c_mstwpdf* pdf_object;
    DSSpiNLO* ff_object;

public:
    ThreadLocalContext(const char* pdf_filename, const char* ff_filename) :
      pdf_object(new c_mstwpdf(pdf_filename)),
      ff_object(new DSSpiNLO(ff_filename)) {};
    ThreadLocalContext(const Context& ctx) :
      pdf_object(new c_mstwpdf(ctx.pdf_filename.c_str())),
      ff_object(new DSSpiNLO(ctx.ff_filename.c_str())) {};
    ThreadLocalContext(const ContextCollection& cc) :
      pdf_object(new c_mstwpdf(cc.options.find("pdf_filename")->second.c_str())),
      ff_object(new DSSpiNLO(cc.options.find("ff_filename")->second.c_str())) {};
    
    ~ThreadLocalContext() {
        delete pdf_object;
        delete ff_object;
    }
};

#endif // _CONTEXT_H_
