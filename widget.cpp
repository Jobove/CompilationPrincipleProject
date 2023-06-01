#include <QFileDialog>
#include <QProcess>
#include <QWidget>
#include <QTableWidget>
#include <QTreeWidget>
#include <stack>
#include <string>
#include <vector>

#include "boost/format.hpp"
#include "BNF.h"
#include "MinimizedDFA.h"
#include "widget.h"
#include "./ui_widget.h"


Widget::Widget(QWidget *parent) : QWidget(parent), ui(new Ui::Widget) {
    ui->setupUi(this);

    connect(ui->select_lex_config_button, SIGNAL(clicked), this, SLOT(on_select_lex_config_button_clicked()));
    connect(ui->generate_lexer_button, SIGNAL(clicked), this, SLOT(on_generate_lexer_button_clicked()));
    connect(ui->select_gcc_folder_button, SIGNAL(clicked), this, SLOT(on_select_gcc_folder_button_clicked()));
    connect(ui->select_source_file_button, SIGNAL(clicked), this, SLOT(on_select_source_file_button_clicked()));
    connect(ui->lexical_analyze_button, SIGNAL(clicked), this, SLOT(on_lexical_analyze_button_clicked()));
    connect(ui->bnf_path_button, SIGNAL(clicked), this, SLOT(on_bnf_path_button_clicked()));
    connect(ui->lex_encode_button, SIGNAL(clicked), this, SLOT(on_lex_encode_button_clicked()));
    connect(ui->grammar_analyse_button, SIGNAL(clicked), this, SLOT(on_grammar_analyse_button_clicked()));
}

Widget::~Widget() {
    delete ui;
}

void Widget::on_select_lex_config_button_clicked() {
    ui->lex_config_input->setText(
            QFileDialog::getOpenFileName(this, tr("选择词法配置文件"), "../Config/", tr("json(*.json)")));
}

void Widget::on_generate_lexer_button_clicked() {
    if (ui->lex_config_input->text().isEmpty())
        return;

    std::string lexer;
    auto lex_parser = std::move(get_lexer(ui->lex_config_input->text().toStdString(), lexer));
    ui->lexer_output->setPlainText(QString::fromStdString(lexer));
    set_up_tables(lex_parser);
}

void Widget::set_up_state_tables(std::vector<MinimizedDFA> &arr, QToolBox *box, int const mode) {
    auto get_list = [&](MinimizedDFA &i, int const mode) {
        switch (mode) {
            case 0: {
                return i.get_nfa_list();
            }
            case 1: {
                return i.get_dfa_list();
            }
            default: {
                return i.get_adjacent_list();
            }
        }
    };

    int count = 0;
    for (auto &i: arr) {
        auto list = get_list(i, mode);
        auto *widget = new QWidget;
        auto *layout = new QVBoxLayout(widget);
        auto *table_widget = new QTableWidget(nullptr);
        std::set<char> char_set;

        for (auto &[u, edges]: list)
            for (auto &[ch, _]: edges)
                char_set.insert(ch);

        table_widget->setRowCount((int) list.size());
        table_widget->setColumnCount((int) char_set.size());
        int j = 0;
        for (auto &ch: char_set)
            table_widget->setHorizontalHeaderItem(j++, new QTableWidgetItem(QString(ch)));

        int row = 0, column;
        for (auto &[u, edges]: list) {
            column = 0;
            for (auto &ch: char_set) {
                if (not edges.contains(ch)) {
                    table_widget->setItem(row, column, new QTableWidgetItem("-"));
                    ++column;
                    continue;
                }
                table_widget->setItem(row, column, new QTableWidgetItem(QString::fromStdString(edges[ch])));
                ++column;
            }
            ++row;
        }

        table_widget->setShowGrid(true);
        table_widget->setGridStyle(Qt::SolidLine);
        layout->addWidget(table_widget);
        box->addItem(widget, QString(mode ? (mode == 1 ? "DFA %1" : " 最小化DFA %1") : "NFA %1").arg(count++));
        table_widget->show();
    }
}

void Widget::set_up_tables(std::vector<MinimizedDFA> &arr) const {
    for (int i = 0; i < ui->nfa_box->count(); ++i)
        ui->nfa_box->removeItem(i), ui->dfa_box->removeItem(i), ui->minimized_dfa_box->removeItem(i);

    set_up_state_tables(arr, ui->nfa_box, 0);
    set_up_state_tables(arr, ui->dfa_box, 1);
    set_up_state_tables(arr, ui->minimized_dfa_box, 2);
}

void Widget::on_select_gcc_folder_button_clicked() {
    ui->gcc_folder_input->setText(QFileDialog::getExistingDirectory(this, tr("选择GCC所在的文件夹"), "C:/"));
}

void Widget::on_select_source_file_button_clicked() {
    ui->source_file_input->setText(
            QFileDialog::getOpenFileName(this, tr("选择源文件"), "../Config/", tr("任意文件(*.*)")));
}

void Widget::on_lexical_analyze_button_clicked() const {
    if (ui->source_file_input->text().isEmpty())
        return;
    string gcc_folder = ui->gcc_folder_input->text().toStdString();
    string source_file = ui->source_file_input->text().toStdString();

    QFile program("tmp.txt");
    program.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate);
    program.write(ui->lexer_output->toPlainText().toUtf8());
    program.close();

    QProcess process;
    process.start(QString::fromStdString(gcc_folder + "/gcc.exe"),
                  QStringList() << "-o" << "output.exe" << program.fileName());
    process.waitForFinished();
    process.close();
    process.start("output.exe",
                  QStringList() << QString::fromStdString(source_file) << QString::fromStdString("output.txt"));

    QFile output("output.txt");
    output.open(QIODevice::ReadOnly | QIODevice::Text | QIODevice::Truncate);
    ui->lex_output->setPlainText(output.readAll());
    output.close();
}

void Widget::on_bnf_path_button_clicked() {
    ui->bnf_path_input->setText(
            QFileDialog::getOpenFileName(this, tr("选择BNF文件"), "../Config/", tr("任意文件(*.*)")));
}

void Widget::on_lex_encode_button_clicked() {
    ui->lex_path_input->setText(
            QFileDialog::getOpenFileName(this, tr("选择词法分析文件"), "../Config/", tr("任意文件(*.*)")));
}

void Widget::set_up_first_or_follow(const std::map<std::string, std::set<std::string>> &first, QTableWidget *table) {
    int max_column = 1;
    table->clear();
    table->setRowCount((int) first.size());

    for (auto &[key, value]: first)
        max_column = std::max(max_column, (int) value.size());
    table->setColumnCount(max_column + 1);

    int row = 0, column;
    for (auto &[key, value]: first) {
        column = 1;
        table->setItem(row, column++, new QTableWidgetItem(QString::fromStdString(key)));
        for (auto &i: value) {
            table->setItem(row, column++, new QTableWidgetItem(QString::fromStdString(i)));
            ++column;
        }
        ++row;
    }
}

void Widget::set_up_LL1(std::map<std::string, std::map<std::string, std::set<std::vector<string>>>> LL1, BNF &bnf,
                        QTableWidget *table) {
    auto terminal = bnf.get_terminal(), non_terminal = bnf.get_non_terminal();

    table->clear();
    table->setColumnCount((int) terminal.size() + 1);
    table->setRowCount((int) non_terminal.size() + 1);

    int column = 1;
    for (auto const &str: terminal) {
        table->setItem(0, column, new QTableWidgetItem(QString::fromStdString(str)));
        ++column;
    }
    int row = 1;
    for (const auto &left: non_terminal) {
        table->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(left)));
        column = 1;
        for (auto const &it: terminal) {
            string item;
            for (const auto &vector: LL1[left][it]) {
                for (auto const &str: vector)
                    item += str + " ";

                item += " | ";
                table->setItem(row, column, new QTableWidgetItem(QString::fromStdString(item)));
            }
            ++column;
        }
        ++row;
    }
}

void Widget::on_grammar_analyse_button_clicked() const {
    if (ui->bnf_path_input->text().isEmpty())
        return;

    string bnf_path = ui->bnf_path_input->text().toStdString(), bnf_content;
    QFile bnf_file(QString::fromStdString(bnf_path));
    bnf_file.open(QIODevice::ReadOnly | QIODevice::Text);
    bnf_content = bnf_file.readAll().toStdString();
    bnf_file.close();

    std::map<string, std::vector<std::vector<string>>> map = parse(bnf_content);
    BNF bnf(map, string(Utils::start));

    bnf.remove_interminable();
    bnf.remove_unreachable();

    auto expressions = bnf.get_expressions();
    set_up_expression_table(expressions, ui->simplified_table);

    bnf.remove_indirect_left_recursion();
    bnf.substitution();
    bnf.remove_interminable();
    bnf.remove_unreachable();

    bnf.extract_left_common_factor_recursive();

    bnf.remove_indirect_left_recursion();

    expressions = bnf.get_expressions();
    set_up_expression_table(expressions, ui->left_removed_table);

    bnf.process_first();
    bnf.process_follow();
    bnf.process_terminal();
    bnf.process_LL1();
    set_up_first_or_follow(bnf.get_first(), ui->first_table);
    set_up_first_or_follow(bnf.get_follow(), ui->follow_table);
    set_up_LL1(bnf.get_LL1(), bnf, ui->ll1_table);

    string content;
    if (ui->lex_path_input->text().isEmpty() and not ui->lexer_output->toPlainText().isEmpty())
        content = ui->lexer_output->toPlainText().toStdString();
    else if (not ui->lex_path_input->text().isEmpty()) {
        QFile lex(ui->lex_path_input->text());
        lex.open(QIODevice::ReadOnly | QIODevice::Text);
        content = lex.readAll().toStdString();
        lex.close();
    } else
        return;

    auto statement = get_tuples(content);
    bnf.generate_syntax_tree(statement);
    set_up_syntax_tree(bnf.get_root(), ui->syntax_tree);
}

void Widget::set_up_syntax_tree(TreeNode const &root, QTreeWidget *tree) {
    tree->clear();

    auto str = [](TreeNode const &node) {
        return (boost::format("(%d%s)") % node.data.show_num %
                (node.data.num.empty() ? "" : ", " + node.data.num)).str();
    };
    std::stack<std::pair<TreeNode *, QTreeWidgetItem *>> s;
    s.emplace(const_cast<TreeNode *>(&root), new QTreeWidgetItem(tree));

    while (not s.empty()) {
        auto [node, item] = s.top();
        s.pop();
        item->setText(0, QString::fromStdString(str(*node)));
        for (auto &i: node->child) {
            auto son = new QTreeWidgetItem(item);
            s.emplace(&i, son);
        }
    }
}

void Widget::set_up_expression_table(map<std::string, std::vector<std::vector<std::string>>> &expressions,
                                     QTableWidget *table) {
    int max_column = 1;

    table->clear();
    table->setRowCount((int) expressions.size());

    for (auto &[left, right]: expressions) {
        max_column = std::max(max_column, static_cast<int>(right.size()));
    }
    table->setColumnCount(max_column + 1);

    int row = 0;
    auto join = [](const std::vector<std::string> &vec) -> std::string {
        std::string result;
        for (auto &i: vec) {
            result += i + " ";
        }
        return result;
    };

    for (auto &[left, right]: expressions) {
        table->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(left)));
        int column = 1;
        for (auto &i: right) {
            table->setItem(row, column, new QTableWidgetItem(QString::fromStdString(join(i))));
            ++column;
        }
        ++row;
    }
}


