#include "database.h"


void Database::execute_cmd(const std::string& sql) const
{
	sqlite3_stmt* stmt;
	sqlite3_prepare_v100(sql, &stmt);
	sqlite3_step(stmt);
	sqlite3_finalize(stmt);
}

int Database::get_int(const std::string& sql) const
{
	Statement stmt;
	stmt.init(*this, sql);
	while (stmt.step() == SQLITE_ROW)
		return stmt.column_int(0);
	throw "effbf28c";
}

array Database::get_ints(const std::string& table_name, const std::string& column_name, const std::string& conditions) const
{
	array result;
	Statement stmt;
	stmt.init(*this, "SELECT " + column_name + " FROM " + table_name + ' ' + conditions + ';');
	while (stmt.step() == SQLITE_ROW)
		result.emplace_back(stmt.column_int(0));
	std::cout << column_name << " loaded from " << table_name << ", size=" << result.size() << '\n';
	return result;
}

std::vector<Deg> Database::load_gen_degs(const std::string& table_name) const
{
	std::vector<Deg> gen_degs;
	Statement stmt;
	stmt.init(*this, "SELECT s, t, v FROM " + table_name + " ORDER BY gen_id;");
	while (stmt.step() == SQLITE_ROW)
		gen_degs.emplace_back(stmt.column_int(0), stmt.column_int(1), stmt.column_int(2));
	std::cout << "gen_degs loaded from " << table_name << ", size=" << gen_degs.size() << '\n';
	return gen_degs;
}

Poly1d Database::load_gen_diffs(const std::string& table_name) const
{
	Poly1d diffs;
	Statement stmt;
	stmt.init(*this, "SELECT gen_diff FROM " + table_name + " ORDER BY gen_id;");
	while (stmt.step() == SQLITE_ROW)
		diffs.push_back(str_to_Poly(stmt.column_str(0)));
	std::cout << "diffs loaded from " << table_name << ", size=" << diffs.size() << '\n';
	return diffs;
}

Poly1d Database::load_gen_reprs(const std::string& table_name) const
{
	Poly1d reprs;
	Statement stmt;
	stmt.init(*this, "SELECT repr FROM " + table_name + " ORDER BY gen_id;");
	while (stmt.step() == SQLITE_ROW)
		reprs.push_back(str_to_Poly(stmt.column_str(0)));
	std::cout << "reprs loaded from " << table_name << ", size=" << reprs.size() << '\n';
	return reprs;
}

Mon2d Database::load_leading_terms(const std::string& table_name) const
{
	Mon2d leadings;
	Statement stmt;
	stmt.init(*this, "SELECT leading_term FROM " + table_name + " ORDER BY t;");
	while (stmt.step() == SQLITE_ROW) {
		Mon mon(str_to_Mon(stmt.column_str(0)));
		if (size_t(mon[0].gen) >= leadings.size())
			leadings.resize(size_t(mon[0].gen) + 1);
		leadings[mon[0].gen].push_back(mon);
	}
	std::cout << "leadings loaded from " << table_name << ", size=" << leadings.size() << '\n';
	return leadings;
}

Poly1d Database::load_gb(const std::string& table_name) const
{
	Poly1d gb;
	Statement stmt;
	stmt.init(*this, "SELECT leading_term, basis FROM " + table_name + " ORDER BY t;");
	while (stmt.step() == SQLITE_ROW) {
		Mon lead(str_to_Mon(stmt.column_str(0)));
		Poly basis(str_to_Poly(stmt.column_str(1)));
		Poly g = add(basis, { lead });
		gb.push_back(std::move(g));
	}
	std::cout << "gb loaded from " << table_name << ", size=" << gb.size() << '\n';
	return gb;
}

std::map<Deg, Mon1d> Database::load_basis(const std::string& table_name) const
{
	std::map<Deg, Mon1d> basis;
	Statement stmt;
	stmt.init(*this, "SELECT s, t, v, mon FROM " + table_name + " ORDER BY mon_id;");
	while (stmt.step() == SQLITE_ROW) {
		Deg d = { stmt.column_int(0), stmt.column_int(1), stmt.column_int(2) };
		basis[d].push_back(str_to_Mon(stmt.column_str(3)));
	}
	std::cout << "basis loaded from " << table_name << ", size=" << basis.size() << '\n';
	return basis;
}

std::map<Deg, array2d> Database::load_mon_diffs_ind(const std::string& table_name) const
{
	std::map<Deg, array2d> diffs_ind;
	Statement stmt;
	stmt.init(*this, "SELECT s, t, v, diff FROM " + table_name + " WHERE diff IS NOT NULL ORDER BY mon_id;");
	while (stmt.step() == SQLITE_ROW) {
		Deg deg = { stmt.column_int(0), stmt.column_int(1), stmt.column_int(2) };
		diffs_ind[deg].push_back(str_to_array(stmt.column_str(3)));
	}
	std::cout << "diffs_ind loaded from " << table_name << ", size=" << diffs_ind.size() << '\n';
	return diffs_ind;
}

std::map<Deg, Poly1d> Database::load_mon_diffs(const std::string& table_name, const std::map<Deg, Mon1d>& basis, int r) const
{
	std::map<Deg, Poly1d> diffs;
	Statement stmt;
	stmt.init(*this, "SELECT s, t, v, diff FROM " + table_name + " WHERE diff IS NOT NULL ORDER BY mon_id;");
	while (stmt.step() == SQLITE_ROW) {
		Deg deg = { stmt.column_int(0), stmt.column_int(1), stmt.column_int(2) };
		array diff_index = str_to_array(stmt.column_str(3));
		diffs[deg].push_back(diff_index.empty() ? Poly{} : indices_to_Poly(std::move(diff_index), basis.at(deg + Deg{ 1, 0, -r })));
	}
	std::cout << "diffs loaded from " << table_name << ", size=" << diffs.size() << '\n';
	return diffs;
}

std::map<Deg, BasisComplex> Database::load_basis_ss(const std::string& table_name, int r) const
{
	std::map<Deg, BasisComplex> basis_ss;
	Statement stmt;
	stmt.init(*this, "SELECT s, t, v, level, base FROM " + table_name + " ;");
	while (stmt.step() == SQLITE_ROW) {
		Deg deg = { stmt.column_int(0), stmt.column_int(1), stmt.column_int(2) };
		int level = stmt.column_int(3);
		if (level <= r)
			basis_ss[deg].boundaries.push_back(str_to_array(stmt.column_str(4)));
		else if (level <= T_MAX - r)
			basis_ss[deg].cycles.push_back(str_to_array(stmt.column_str(4)));
	}
	std::cout << "basis_ss loaded from " << table_name << ", size=" << basis_ss.size() << '\n';
	return basis_ss;
}

void Database::save_generators(const std::string& table_name, const std::vector<Deg>& gen_degs, const Poly1d& gen_reprs, size_t i_start) const
{
	Statement stmt_update_generators;
	stmt_update_generators.init(*this, "INSERT INTO " + table_name + " (gen_id, repr, s, t, v) VALUES (?1, ?2, ?3, ?4, ?5);");

	for (size_t i = i_start; i < gen_degs.size(); ++i) {
		stmt_update_generators.bind_int(1, (int)i);
		stmt_update_generators.bind_str(2, Poly_to_str(gen_reprs[i]));
		stmt_update_generators.bind_int(3, gen_degs[i].s);
		stmt_update_generators.bind_int(4, gen_degs[i].t);
		stmt_update_generators.bind_int(5, gen_degs[i].v);
		stmt_update_generators.step_and_reset();
	}
	std::cout << gen_degs.size() << " generators are inserted into " + table_name + "!\n";
}

void Database::save_gb(const std::string& table_name, const Poly1d& gb, const std::vector<Deg>& gen_degs, size_t i_start) const
{
	Statement stmt_update_relations;
	stmt_update_relations.init(*this, "INSERT INTO " + table_name + " (leading_term, basis, s, t, v) VALUES (?1, ?2, ?3, ?4, ?5);");

	for (size_t i = i_start; i < gb.size(); ++i) {
		Deg deg = get_deg(gb[i], gen_degs);
		stmt_update_relations.bind_str(1, Mon_to_str(gb[i].front()));
		stmt_update_relations.bind_str(2, Poly_to_str(gb[i].begin() + 1, gb[i].end()));
		stmt_update_relations.bind_int(3, deg.s);
		stmt_update_relations.bind_int(4, deg.t);
		stmt_update_relations.bind_int(5, deg.v);
		stmt_update_relations.step_and_reset();
	}
	std::cout << gb.size() << " relations are inserted into " + table_name + "!\n";
}

void Database::save_basis(const std::string& table_name, const std::map<Deg, Mon1d>& basis) const
{
	Statement stmt;
	stmt.init(*this, "INSERT INTO " + table_name + " (mon, s, t, v) VALUES (?1, ?2, ?3, ?4);");

	for (auto& [deg, basis_d] : basis) {
		for (auto& m : basis_d) {
			stmt.bind_str(1, Mon_to_str(m));
			stmt.bind_int(2, deg.s);
			stmt.bind_int(3, deg.t);
			stmt.bind_int(4, deg.v);
			stmt.step_and_reset();
		}
	}
	std::cout << "basis is inserted, number of degrees=" << basis.size() << '\n';
}

void Database::save_basis(const std::string& table_name, const std::map<Deg, Mon1d>& basis, const std::map<Deg, array2d>& mon_reprs) const
{
	Statement stmt;
	stmt.init(*this, "INSERT INTO " + table_name + " (mon, repr, s, t, v) VALUES (?1, ?2, ?3, ?4);");

	for (auto& [deg, basis_d] : basis) {
		for (size_t i = 0; i < basis_d.size(); ++i){
			stmt.bind_str(1, Mon_to_str(basis_d[i]));
			stmt.bind_str(2, array_to_str(mon_reprs.at(deg)[i]));
			stmt.bind_int(3, deg.s);
			stmt.bind_int(4, deg.t);
			stmt.bind_int(5, deg.v);
			stmt.step_and_reset();
		}
	}
	std::cout << "basis is inserted into " + table_name + ", number of degrees=" << basis.size() << '\n';
}

void Database::save_basis(const std::string& table_name, const std::map<Deg, Mon1d>& basis, const std::map<Deg, Poly1d>& mon_reprs) const
{
	Statement stmt;
	stmt.init(*this, "INSERT INTO " + table_name + " (mon, repr, s, t, v) VALUES (?1, ?2, ?3, ?4, ?5);");

	for (auto& [deg, basis_d] : basis) {
		for (size_t i = 0; i < basis_d.size(); ++i) {
			stmt.bind_str(1, Mon_to_str(basis_d[i]));
			stmt.bind_str(2, Poly_to_str(mon_reprs.at(deg)[i]));
			stmt.bind_int(3, deg.s);
			stmt.bind_int(4, deg.t);
			stmt.bind_int(5, deg.v);
			stmt.step_and_reset();
		}
	}
	std::cout << "basis is inserted into " + table_name + ", number of degrees=" << basis.size() << '\n';
}

void Database::save_basis_ss(const std::string& table_name, const std::map<Deg, BasisSS>& basis_ss) const
{
	Statement stmt;
	stmt.init(*this, "INSERT INTO " + table_name + " (base, diff, level, s, t, v) VALUES (?1, ?2, ?3, ?4, ?5, ?6);");

	for (const auto& [deg, basis_ss_d] : basis_ss) {
		for (size_t i = 0; i < basis_ss_d.basis_ind.size(); ++i) {
			stmt.bind_str(1, array_to_str(basis_ss_d.basis_ind[i]));
			stmt.bind_str(2, array_to_str(basis_ss_d.diffs_ind[i]));
			stmt.bind_int(3, basis_ss_d.levels[i]);
			stmt.bind_int(4, deg.s);
			stmt.bind_int(5, deg.t);
			stmt.bind_int(6, deg.v);
			stmt.step_and_reset();
		}
	}
	std::cout << "basis_ss is inserted into " + table_name + ", number of degrees=" << basis_ss.size() << '\n';
}
