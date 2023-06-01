#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QToolBox>
#include <QTableWidget>
#include <QTreeWidget>

#include "LexParser.h"
#include "BNF.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget {
Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);

    ~Widget();

    Ui::Widget *ui;

private slots:

    void on_select_lex_config_button_clicked();

    void on_generate_lexer_button_clicked();

    void on_select_gcc_folder_button_clicked();

    void on_select_source_file_button_clicked();

    void on_lexical_analyze_button_clicked() const;

    void on_bnf_path_button_clicked();

    void on_lex_encode_button_clicked();

    void on_grammar_analyse_button_clicked() const;

private:
    void set_up_tables(std::vector<MinimizedDFA> &arr) const;

    static void set_up_expression_table(std::map<string, std::vector<std::vector<std::string>>> &expressions,
                                        QTableWidget *table);

    static void set_up_first_or_follow(const std::map<std::string, std::set<std::string>> &first,
                                       QTableWidget *table);

    static void set_up_state_tables(std::vector<MinimizedDFA> &arr, QToolBox *box, int mode);

    static void set_up_syntax_tree(TreeNode const &root, QTreeWidget *tree);

    static void set_up_LL1(std::map<std::string, std::map<std::string, std::set<std::vector<string>>>> LL1, BNF &bnf,
                           QTableWidget *table);
};

#endif // WIDGET_H
