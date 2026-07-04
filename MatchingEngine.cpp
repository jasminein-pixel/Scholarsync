#include <nanodbc/nanodbc.h>
#include <rapidfuzz-cpp/rapidfuzz/fuzz.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <cmath>

using namespace std;

// ─────────────────────────────────────────
//  MATCHING ENGINE CLASS
// ─────────────────────────────────────────
class MatchingEngine {
private:
    unordered_map<string, string> synonymMap;
    unordered_map<string, string> categoryMap;

    // ── Normalize: lowercase + expand abbreviations ──
    string normalize(string skill) {
        // convert to lowercase
        transform(skill.begin(), skill.end(), skill.begin(), ::tolower);
        // trim whitespace
        skill.erase(0, skill.find_first_not_of(" "));
        skill.erase(skill.find_last_not_of(" ") + 1);
        // expand abbreviation if found
        if (synonymMap.count(skill))
            return synonymMap[skill];
        return skill;
    }

    // ── Semantic score between two normalized skills ──
    float semanticScore(string a, string b) {
        a = normalize(a);
        b = normalize(b);

        // Layer 1: exact match
        if (a == b) return 1.0f;

        // Layer 2: same category
        if (categoryMap.count(a) && categoryMap.count(b))
            if (categoryMap[a] == categoryMap[b])
                return 0.70f;

        // Layer 3: RapidFuzz fuzzy match
        double fuzzy = rapidfuzz::fuzz::ratio(a, b);
        if (fuzzy >= 80.0)
            return (float)(fuzzy / 100.0);

        return 0.0f;
    }

    // ── Proficiency factor ──
    float proficiencyFactor(int studentProf, int requiredProf) {
        if (requiredProf == 0) return 1.0f;
        if (studentProf >= requiredProf) return 1.0f;
        return (float)studentProf / (float)requiredProf;
    }

public:
    // ── Constructor: load synonym and category maps ──
    MatchingEngine() {

        // SYNONYM MAP — abbreviations to full form
        synonymMap["ml"]   = "machine learning";
        synonymMap["ai"]   = "artificial intelligence";
        synonymMap["dl"]   = "deep learning";
        synonymMap["nlp"]  = "natural language processing";
        synonymMap["cv"]   = "computer vision";
        synonymMap["oop"]  = "object oriented programming";
        synonymMap["dsa"]  = "data structures and algorithms";
        synonymMap["db"]   = "database";
        synonymMap["os"]   = "operating systems";
        synonymMap["js"]   = "javascript";
        synonymMap["ts"]   = "typescript";
        synonymMap["cpp"]  = "c++";
        synonymMap["ui"]   = "user interface design";
        synonymMap["ux"]   = "user experience design";
        synonymMap["devops"] = "development operations";
        synonymMap["se"]   = "software engineering";

        // CATEGORY MAP — related skills grouped together
        // AI / ML domain
        categoryMap["machine learning"]          = "AI";
        categoryMap["artificial intelligence"]   = "AI";
        categoryMap["deep learning"]             = "AI";
        categoryMap["neural networks"]           = "AI";
        categoryMap["natural language processing"] = "AI";
        categoryMap["computer vision"]           = "AI";
        categoryMap["data science"]              = "AI";
        categoryMap["data analysis"]             = "AI";
        categoryMap["reinforcement learning"]    = "AI";
        categoryMap["generative ai"]             = "AI";

        // Web development domain
        categoryMap["web development"]   = "Web";
        categoryMap["frontend"]          = "Web";
        categoryMap["backend"]           = "Web";
        categoryMap["fullstack"]         = "Web";
        categoryMap["javascript"]        = "Web";
        categoryMap["typescript"]        = "Web";
        categoryMap["react"]             = "Web";
        categoryMap["nodejs"]            = "Web";
        categoryMap["html"]              = "Web";
        categoryMap["css"]               = "Web";
        categoryMap["django"]            = "Web";
        categoryMap["flask"]             = "Web";

        // Systems domain
        categoryMap["operating systems"]         = "Systems";
        categoryMap["linux"]                     = "Systems";
        categoryMap["networking"]                = "Systems";
        categoryMap["cybersecurity"]             = "Systems";
        categoryMap["embedded systems"]          = "Systems";
        categoryMap["computer architecture"]     = "Systems";
        categoryMap["distributed systems"]       = "Systems";

        // Database domain
        categoryMap["database"]          = "Database";
        categoryMap["sql"]               = "Database";
        categoryMap["mysql"]             = "Database";
        categoryMap["mongodb"]           = "Database";
        categoryMap["postgresql"]        = "Database";
        categoryMap["database design"]   = "Database";

        // Software Engineering domain
        categoryMap["object oriented programming"] = "SE";
        categoryMap["software engineering"]        = "SE";
        categoryMap["design patterns"]             = "SE";
        categoryMap["agile"]                       = "SE";
        categoryMap["development operations"]      = "SE";
        categoryMap["git"]                         = "SE";
        categoryMap["testing"]                     = "SE";

        // Data-focused languages
        categoryMap["python"] = "DataLang";
        categoryMap["r"]      = "DataLang";
        categoryMap["julia"]  = "DataLang";

        // Systems languages
        categoryMap["c++"]  = "SysLang";
        categoryMap["c"]    = "SysLang";
        categoryMap["rust"] = "SysLang";

        // Mobile domain
        categoryMap["android"]        = "Mobile";
        categoryMap["ios"]            = "Mobile";
        categoryMap["flutter"]        = "Mobile";
        categoryMap["react native"]   = "Mobile";
        categoryMap["mobile development"] = "Mobile";

        // Electrical / Engineering domain
        categoryMap["embedded system designing"] = "EE";
        categoryMap["circuit design"]            = "EE";
        categoryMap["fpga"]                      = "EE";
        categoryMap["microcontrollers"]          = "EE";
        categoryMap["signal processing"]         = "EE";
        categoryMap["power systems"]             = "EE";
    }

    // ─────────────────────────────────────────
    //  CORE SCORE: Student vs Project
    // ─────────────────────────────────────────
    int calculateScore(int SID, int PID, nanodbc::connection& conn) {
        try {
            // ── Fetch student skills ──
            vector<pair<string, int>> studentSkills;
            string skillQuery =
                "SELECT SkillName, Proficiency FROM SkillList WHERE SID = ?";
            auto skillStmt = nanodbc::statement(conn, skillQuery);
            skillStmt.bind(0, &SID);
            auto skillResult = nanodbc::execute(skillStmt);
            while (skillResult.next())
                studentSkills.push_back({
                    skillResult.get<string>(0),
                    skillResult.get<int>(1)
                });

            // ── Fetch project requirements ──
            vector<pair<string, int>> requirements;
            string reqQuery =
                "SELECT skillName, requiredProficiency FROM skillRequirement WHERE PID = ?";
            auto reqStmt = nanodbc::statement(conn, reqQuery);
            reqStmt.bind(0, &PID);
            auto reqResult = nanodbc::execute(reqStmt);
            while (reqResult.next())
                requirements.push_back({
                    reqResult.get<string>(0),
                    reqResult.get<int>(1)
                });

            // ── Fetch student interests (from preference field) ──
            vector<string> studentInterests;
            string intQuery =
                "SELECT Preference FROM StudentDetails WHERE SID = ?";
            auto intStmt = nanodbc::statement(conn, intQuery);
            intStmt.bind(0, &SID);
            auto intResult = nanodbc::execute(intStmt);
            if (intResult.next())
                studentInterests.push_back(intResult.get<string>(0));

            // ── Fetch project preference ──
            string projectPref = "";
            string prefQuery =
                "SELECT preference, department FROM ProjectDetails WHERE PID = ?";
            auto prefStmt = nanodbc::statement(conn, prefQuery);
            prefStmt.bind(0, &PID);
            auto prefResult = nanodbc::execute(prefStmt);
            if (prefResult.next()) {
                projectPref = prefResult.get<string>(0);
            }

            // ── If no requirements, return 0 ──
            if (requirements.empty()) return 0;

            // ── Calculate skill score ──
            float totalSkillScore = 0.0f;
            for (auto& [reqSkill, reqProf] : requirements) {
                float bestContribution = 0.0f;
                for (auto& [stuSkill, stuProf] : studentSkills) {
                    float semantic  = semanticScore(stuSkill, reqSkill);
                    float profFactor = proficiencyFactor(stuProf, reqProf);
                    float contribution = semantic * profFactor;
                    if (contribution > bestContribution)
                        bestContribution = contribution;
                }
                totalSkillScore += bestContribution;
            }
            float avgSkillScore = totalSkillScore / requirements.size();

            // ── Calculate interest score ──
            float interestScore = 0.0f;
            if (!studentInterests.empty() && !projectPref.empty()) {
                float best = semanticScore(studentInterests[0], projectPref);
                interestScore = best;
            }

            // ── Combine: 70% skills, 30% interests ──
            float raw   = (avgSkillScore * 0.7f) + (interestScore * 0.3f);
            int   final = max(1, (int)ceil(raw * 10));
            return min(final, 10);  // cap at 10
        }
        catch (const nanodbc::database_error& e) {
            cout << "MatchingEngine error: " << e.what() << "\n";
            return 0;
        }
    }

    // ─────────────────────────────────────────
    //  STUDENT SIDE: Ranked projects for a student
    // ─────────────────────────────────────────
    vector<pair<int, int>> getRankedProjects(int SID, nanodbc::connection& conn) {
        vector<pair<int, int>> results;  // {score, PID}
        try {
            // Get all active projects
            string query = "SELECT PID FROM ProjectDetails WHERE status = 'Active'";
            auto result = nanodbc::execute(conn, query);
            while (result.next()) {
                int PID   = result.get<int>(0);
                int score = calculateScore(SID, PID, conn);
                if (score > 0)
                    results.push_back({score, PID});
            }
            // Sort descending by score
            sort(results.begin(), results.end(),
                [](auto& a, auto& b) { return a.first > b.first; });
        }
        catch (const nanodbc::database_error& e) {
            cout << "getRankedProjects error: " << e.what() << "\n";
        }
        return results;
    }

    // ─────────────────────────────────────────
    //  PROFESSOR SIDE: Ranked students for a project
    // ─────────────────────────────────────────
    vector<pair<int, int>> getRankedStudents(int PID, nanodbc::connection& conn) {
        vector<pair<int, int>> results;  // {score, SID}
        try {
            // Get all students who applied to this project
            string query =
                "SELECT SID FROM Applications WHERE PID = ? AND Status = 'reviewing'";
            auto stmt = nanodbc::statement(conn, query);
            stmt.bind(0, &PID);
            auto result = nanodbc::execute(stmt);
            while (result.next()) {
                int SID   = result.get<int>(0);
                int score = calculateScore(SID, PID, conn);
                results.push_back({score, SID});
            }
            // Sort descending by score
            sort(results.begin(), results.end(),
                [](auto& a, auto& b) { return a.first > b.first; });
        }
        catch (const nanodbc::database_error& e) {
            cout << "getRankedStudents error: " << e.what() << "\n";
        }
        return results;
    }

    // ─────────────────────────────────────────
    //  UPDATE ENGINE SCORE IN DATABASE
    // ─────────────────────────────────────────
    void updateEngineScore(int SID, int PID, int score, nanodbc::connection& conn) {
        try {
            string query =
                "UPDATE Applications SET EngineScore = ? WHERE SID = ? AND PID = ?";
            auto stmt = nanodbc::statement(conn, query);
            stmt.bind(0, &score);
            stmt.bind(1, &SID);
            stmt.bind(2, &PID);
            nanodbc::execute(stmt);
        }
        catch (const nanodbc::database_error& e) {
            cout << "updateEngineScore error: " << e.what() << "\n";
        }
    }
};

// ─────────────────────────────────────────
//  CONNECTION HELPER
// ─────────────────────────────────────────
nanodbc::connection getConnection() {
    return nanodbc::connection(
        "DRIVER={ODBC Driver 18 for SQL Server};"
        "SERVER=scholarsync.database.windows.net;"
        "DATABASE=ScholarSync;"
        "UID=scholarsync;"
        "PWD=YOUR_PASSWORD_HERE;"
        "Encrypt=yes;"
        "Connection Timeout=30");
}

// ─────────────────────────────────────────
//  MAIN — test both directions
// ─────────────────────────────────────────
int main() {
    try {
        auto conn = getConnection();
        MatchingEngine engine;

        cout << "===== ScholarSync Matching Engine =====\n\n";

        // ── STUDENT SIDE: Rahul (SID=1) sees ranked projects ──
        cout << "---- Projects ranked for Student SID=1 (Rahul) ----\n";
        auto rankedProjects = engine.getRankedProjects(1, conn);
        if (rankedProjects.empty()) {
            cout << "No matching projects found.\n";
        } else {
            for (auto& [score, PID] : rankedProjects) {
                // Fetch project name
                string q = "SELECT ProjectName FROM ProjectDetails WHERE PID = ?";
                auto s = nanodbc::statement(conn, q);
                s.bind(0, &PID);
                auto r = nanodbc::execute(s);
                string projectName = r.next() ? r.get<string>(0) : "Unknown";

                cout << "Score: " << score << "/10"
                     << " | Project: " << projectName
                     << " (PID: " << PID << ")\n";

                // Update score in Applications table
                engine.updateEngineScore(1, PID, score, conn);
            }
        }

        cout << "\n";

        // ── PROFESSOR SIDE: Project 1 ranked applicants ──
        cout << "---- Applicants ranked for Project PID=1 ----\n";
        auto rankedStudents = engine.getRankedStudents(1, conn);
        if (rankedStudents.empty()) {
            cout << "No applicants found.\n";
        } else {
            for (auto& [score, SID] : rankedStudents) {
                // Fetch student name
                string q = "SELECT Name FROM StudentDetails WHERE SID = ?";
                auto s = nanodbc::statement(conn, q);
                s.bind(0, &SID);
                auto r = nanodbc::execute(s);
                string studentName = r.next() ? r.get<string>(0) : "Unknown";

                cout << "Score: " << score << "/10"
                     << " | Student: " << studentName
                     << " (SID: " << SID << ")\n";
            }
        }

    }
    catch (const nanodbc::database_error& e) {
        cout << "Connection error: " << e.what() << "\n";
        cout << "Native: " << e.native() << "\n";
        if (e.native() == 258)
            cout << "Azure sleeping, try again.\n";
    }

    return 0;
}