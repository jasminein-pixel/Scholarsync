#include "MatchingEngine.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <cmath>

// ─────────────────────────────────────────
//  Constructor — load synonym + category maps
// ─────────────────────────────────────────
MatchingEngine::MatchingEngine()
{
    // SYNONYM MAP — abbreviations to full form
    synonymMap["ml"]     = "machine learning";
    synonymMap["ai"]     = "artificial intelligence";
    synonymMap["dl"]     = "deep learning";
    synonymMap["nlp"]    = "natural language processing";
    synonymMap["cv"]     = "computer vision";
    synonymMap["oop"]    = "object oriented programming";
    synonymMap["dsa"]    = "data structures and algorithms";
    synonymMap["db"]     = "database";
    synonymMap["os"]     = "operating systems";
    synonymMap["js"]     = "javascript";
    synonymMap["ts"]     = "typescript";
    synonymMap["cpp"]    = "c++";
    synonymMap["ui"]     = "user interface design";
    synonymMap["ux"]     = "user experience design";
    synonymMap["se"]     = "software engineering";
    synonymMap["devops"] = "development operations";
    synonymMap["ce"]     = "computer engineering";

    // CATEGORY MAP — related skills → same domain
    // AI/ML
    categoryMap["machine learning"]             = "AI";
    categoryMap["artificial intelligence"]      = "AI";
    categoryMap["deep learning"]                = "AI";
    categoryMap["neural networks"]              = "AI";
    categoryMap["natural language processing"]  = "AI";
    categoryMap["computer vision"]              = "AI";
    categoryMap["data science"]                 = "AI";
    categoryMap["data analysis"]                = "AI";
    categoryMap["reinforcement learning"]       = "AI";
    categoryMap["generative ai"]                = "AI";

    // Web
    categoryMap["web development"]  = "Web";
    categoryMap["frontend"]         = "Web";
    categoryMap["backend"]          = "Web";
    categoryMap["fullstack"]        = "Web";
    categoryMap["javascript"]       = "Web";
    categoryMap["typescript"]       = "Web";
    categoryMap["react"]            = "Web";
    categoryMap["nodejs"]           = "Web";
    categoryMap["html"]             = "Web";
    categoryMap["css"]              = "Web";
    categoryMap["django"]           = "Web";
    categoryMap["flask"]            = "Web";

    // Systems
    categoryMap["operating systems"]         = "Systems";
    categoryMap["linux"]                     = "Systems";
    categoryMap["networking"]                = "Systems";
    categoryMap["cybersecurity"]             = "Systems";
    categoryMap["embedded systems"]          = "Systems";
    categoryMap["computer architecture"]     = "Systems";
    categoryMap["distributed systems"]       = "Systems";
    categoryMap["embedded system designing"] = "Systems";

    // Database
    categoryMap["database"]        = "Database";
    categoryMap["sql"]             = "Database";
    categoryMap["mysql"]           = "Database";
    categoryMap["mongodb"]         = "Database";
    categoryMap["postgresql"]      = "Database";
    categoryMap["database design"] = "Database";

    // Software Engineering
    categoryMap["object oriented programming"] = "SE";
    categoryMap["software engineering"]        = "SE";
    categoryMap["design patterns"]             = "SE";
    categoryMap["agile"]                       = "SE";
    categoryMap["development operations"]      = "SE";
    categoryMap["git"]                         = "SE";
    categoryMap["testing"]                     = "SE";

    // Data languages
    categoryMap["python"] = "DataLang";
    categoryMap["r"]      = "DataLang";
    categoryMap["julia"]  = "DataLang";

    // Systems languages
    categoryMap["c++"]  = "SysLang";
    categoryMap["c"]    = "SysLang";
    categoryMap["rust"] = "SysLang";

    // Mobile
    categoryMap["android"]            = "Mobile";
    categoryMap["ios"]                = "Mobile";
    categoryMap["flutter"]            = "Mobile";
    categoryMap["react native"]       = "Mobile";
    categoryMap["mobile development"] = "Mobile";

    // Electrical Engineering
    categoryMap["embedded system designing"] = "EE";
    categoryMap["circuit design"]            = "EE";
    categoryMap["fpga"]                      = "EE";
    categoryMap["microcontrollers"]          = "EE";
    categoryMap["signal processing"]         = "EE";
    categoryMap["power systems"]             = "EE";
    categoryMap["faraday"]                   = "EE";
    categoryMap["electromagnetic"]           = "EE";
    categoryMap["induction"]                 = "EE";
}

// ─────────────────────────────────────────
//  Normalize — lowercase + expand abbreviations
// ─────────────────────────────────────────
QString MatchingEngine::normalize(const QString &skill)
{
    QString s = skill.trimmed().toLower();
    if (synonymMap.contains(s))
        return synonymMap[s];
    return s;
}

// ─────────────────────────────────────────
//  Semantic score between two skills
// ─────────────────────────────────────────
float MatchingEngine::semanticScore(const QString &a, const QString &b)
{
    QString na = normalize(a);
    QString nb = normalize(b);

    // Layer 1: exact match
    if (na == nb) return 1.0f;

    // Layer 2: same category
    if (categoryMap.contains(na) && categoryMap.contains(nb))
        if (categoryMap[na] == categoryMap[nb])
            return 0.70f;

    // Layer 3: fuzzy — substring check
    if (na.contains(nb) || nb.contains(na))
        return 0.60f;

    // Layer 4: character similarity ratio
    int longer = qMax(na.length(), nb.length());
    if (longer == 0) return 1.0f;

    int matches = 0;
    QString shorter   = na.length() < nb.length() ? na : nb;
    QString longerStr = na.length() >= nb.length() ? na : nb;
    for (QChar c : shorter)
        if (longerStr.contains(c)) matches++;

    float ratio = (float)matches / longer;
    if (ratio >= 0.8f) return ratio;

    return 0.0f;
}

// ─────────────────────────────────────────
//  Proficiency factor
// ─────────────────────────────────────────
float MatchingEngine::proficiencyFactor(int studentProf, int requiredProf)
{
    if (requiredProf == 0) return 1.0f;
    if (studentProf >= requiredProf) return 1.0f;
    return (float)studentProf / (float)requiredProf;
}

// ─────────────────────────────────────────
//  Calculate score: one student vs one project
// ─────────────────────────────────────────
int MatchingEngine::calculateScore(int SID, int PID, QSqlDatabase &db)
{
    // Fetch student skills
    QVector<QPair<QString,int>> studentSkills;
    QSqlQuery skillQ(db);
    skillQ.prepare("SELECT SkillName, Proficiency FROM SkillList WHERE SID = ?");
    skillQ.addBindValue(SID);
    if (skillQ.exec()) {
        while (skillQ.next())
            studentSkills.append({skillQ.value(0).toString(), skillQ.value(1).toInt()});
    }

    // Fetch project requirements
    QVector<QPair<QString,int>> requirements;
    QSqlQuery reqQ(db);
    reqQ.prepare("SELECT skillName, requiredProficiency FROM skillRequirement WHERE PID = ?");
    reqQ.addBindValue(PID);
    if (reqQ.exec()) {
        while (reqQ.next())
            requirements.append({reqQ.value(0).toString(), reqQ.value(1).toInt()});
    }

    // Fetch student preference
    QSqlQuery intQ(db);
    intQ.prepare("SELECT Preference FROM StudentDetails WHERE SID = ?");
    intQ.addBindValue(SID);
    QString studentInterest;
    if (intQ.exec() && intQ.next())
        studentInterest = intQ.value(0).toString();

    // Fetch project preference
    QSqlQuery prefQ(db);
    prefQ.prepare("SELECT preference FROM ProjectDetails WHERE PID = ?");
    prefQ.addBindValue(PID);
    QString projectPref;
    if (prefQ.exec() && prefQ.next())
        projectPref = prefQ.value(0).toString();

    if (requirements.isEmpty()) return 0;

    // Skill score (70% weight)
    float totalSkillScore = 0.0f;
    for (auto &[reqSkill, reqProf] : requirements) {
        float bestContribution = 0.0f;
        for (auto &[stuSkill, stuProf] : studentSkills) {
            float semantic    = semanticScore(stuSkill, reqSkill);
            float profFactor  = proficiencyFactor(stuProf, reqProf);
            float contribution = semantic * profFactor;
            if (contribution > bestContribution)
                bestContribution = contribution;
        }
        totalSkillScore += bestContribution;
    }
    float avgSkillScore = totalSkillScore / requirements.size();

    // Interest score (30% weight)
    float interestScore = 0.0f;
    if (!studentInterest.isEmpty() && !projectPref.isEmpty())
        interestScore = semanticScore(studentInterest, projectPref);

    float raw   = (avgSkillScore * 0.7f) + (interestScore * 0.3f);
    int   final = qMax(1, (int)ceil(raw * 10));
    return qMin(final, 10);
}

// ─────────────────────────────────────────
//  Get ranked projects for a student
// ─────────────────────────────────────────
QVector<QPair<int,int>> MatchingEngine::getRankedProjects(int SID, QSqlDatabase &db)
{
    QVector<QPair<int,int>> results;

    QSqlQuery q(db);
    q.exec("SELECT PID FROM ProjectDetails WHERE status = 'Active' AND vacantSpot > 0");
    while (q.next()) {
        int PID   = q.value(0).toInt();
        int score = calculateScore(SID, PID, db);
        if (score > 0)
            results.append({score, PID});
    }

    std::sort(results.begin(), results.end(),
        [](auto &a, auto &b){ return a.first > b.first; });

    return results;
}

// ─────────────────────────────────────────
//  Get ranked students for a project
// ─────────────────────────────────────────
QVector<QPair<int,int>> MatchingEngine::getRankedStudents(int PID, QSqlDatabase &db)
{
    QVector<QPair<int,int>> results;

    QSqlQuery q(db);
    q.prepare("SELECT SID FROM Applications WHERE PID = ? AND Status = 'reviewing'");
    q.addBindValue(PID);
    if (q.exec()) {
        while (q.next()) {
            int SID   = q.value(0).toInt();
            int score = calculateScore(SID, PID, db);
            results.append({score, SID});
        }
    }

    std::sort(results.begin(), results.end(),
        [](auto &a, auto &b){ return a.first > b.first; });

    return results;
}

// ─────────────────────────────────────────
//  Update engine score in Applications
// ─────────────────────────────────────────
void MatchingEngine::updateEngineScore(int SID, int PID, int score, QSqlDatabase &db)
{
    QSqlQuery q(db);
    q.prepare("UPDATE Applications SET EngineScore = ? WHERE SID = ? AND PID = ?");
    q.addBindValue(score);
    q.addBindValue(SID);
    q.addBindValue(PID);
    q.exec();
}
