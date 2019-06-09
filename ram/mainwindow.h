#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <vector>
#include <QtDebug>
#include <QPainter>
#include <QElapsedTimer>
#include <QString>

namespace Ui {
	class MainWindow;
}

class MainWindow : public QMainWindow
{
		Q_OBJECT

	public:
		explicit MainWindow(QWidget *parent = nullptr);
		~MainWindow();

	private:
		Ui::MainWindow *ui;
	public:
		int RAMSIZE = 640;
		const std::vector<std::pair<int, int>> steps = {{1, 130}, {2, 60}, {3, 100}, {2, -60}, {4, 200}, {3, -100}, {1, -130}, {5, 140}, {6, 60}, {7, 50}, {6, -60}};
		std::vector<std::pair<int, int>> remainRam = {{0, RAMSIZE}};
		std::vector<std::pair<int, std::pair<int, int>>> used = {};
		int dispatchMode = 0;
		unsigned int nowStep = 0;

	public:
		void draw();
		void reset();
		void setStutusBar();
		void freeRam(int begin, int end);
		void run_a_step(std::pair<int, int>);
		bool eventFilter(QObject *watched, QEvent *event);
};

#endif // MAINWINDOW_H
