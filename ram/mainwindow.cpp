#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow){
	ui->setupUi(this);
	ui->label_graph->installEventFilter(this);
	setStutusBar();
	connect(ui->radioButton_first, &QRadioButton::toggled, this, [&]{ dispatchMode = ui->radioButton_first->isChecked() ? 0 : ui->radioButton_best->isChecked() ? 1 : 2; });
	connect(ui->radioButton_best,  &QRadioButton::toggled, this, [&]{ dispatchMode = ui->radioButton_first->isChecked() ? 0 : ui->radioButton_best->isChecked() ? 1 : 2; });
	connect(ui->pushButton_next, &QPushButton::clicked, this, [&]{
		if(nowStep < 11) {
			ui->tableWidget->item(int(nowStep), 0)->setBackgroundColor(QColor(0,190,190));
			this->run_a_step(steps[nowStep]);
			setStutusBar();
			nowStep++;
		}
		else{
			ui->pushButton_next->setEnabled(false);
			ui->pushButton_end->setEnabled(false);
		}
	});
	connect(ui->pushButton_end, &QPushButton::clicked, this,[&]{
		ui->pushButton_next->setEnabled(false);
		ui->pushButton_reset->setEnabled(false);
		for( ; nowStep < 11; nowStep++){
			ui->tableWidget->item(int(nowStep), 0)->setBackgroundColor(QColor(0,190,190));
			this->run_a_step(steps[nowStep]);
			setStutusBar();
			QElapsedTimer t;
			t.start();
			while(t.elapsed() < 350) QCoreApplication::processEvents();
		}
		ui->pushButton_end->setEnabled(false);
		ui->pushButton_reset->setEnabled(true);
	});
	connect(ui->pushButton_reset, &QPushButton::clicked, this, &MainWindow::reset);
}

MainWindow::~MainWindow(){
	delete ui;
}

void MainWindow::reset(){
	nowStep = 0;
	used.clear();
	setStutusBar();
	remainRam = {{0, RAMSIZE}};
	ui->pushButton_next->setEnabled(true);
	ui->pushButton_end->setEnabled(true);
	this->resize(this->width(), this->height() + 1);
	this->resize(this->width(), this->height() - 1);
	for(int i = 0; i < 11; i++) ui->tableWidget->item(i, 0)->setBackgroundColor(QColor(255, 255, 255));
}


void MainWindow::freeRam(int begin, int end){
	unsigned int before = 8000, after = 8000;
	for (unsigned int j = 0; j < remainRam.size(); j++) {
		if (remainRam[j].second == begin) before = j;
		if (remainRam[j].first  == end  ) after = j;
	}
	if (before == 8000 && after == 8000)      remainRam.push_back({begin, end});
	else if (before != 8000 && after == 8000) remainRam[before].second = end;
	else if (before == 8000 && after != 8000) remainRam[after].first = begin;
	else {
		remainRam[before].second = remainRam[after].second;
		remainRam.erase(remainRam.begin() + after);
	}
}


void MainWindow::setStutusBar(){
	int usedRamCount = 0;
	for (auto i : used) usedRamCount += (i.second.second - i.second.first);
	int usedRamRercentage = 100 * usedRamCount / RAMSIZE;
	QString out = "内存已使用 " + QString::number(usedRamRercentage) + "%， " + QString::number(usedRamCount) + " / 640 KB.";
	statusBar()->showMessage(out, 10000);
}

void MainWindow::run_a_step(std::pair<int, int> request){
	if (request.second > 0) {
		std::sort(remainRam.begin(), remainRam.end(), [=](std::pair<int, int> i, std::pair<int, int> j){
			return dispatchMode == 0 ?            i.first  <  j.first :
				   dispatchMode == 1 ? i.second - i.first  <  j.second - j.first :
									   i.second - i.first  >  j.second - j.first ;
		});
		for (auto &i : remainRam) if (i.second - i.first > request.second){
			used.push_back({request.first, {i.first, i.first + request.second}});
			i.first += request.second;
			break;
		}
	}
	else{
		unsigned int index = 0;
		for (unsigned int i = 0; i < used.size(); i++) if (used[i].first == request.first) {
			index = i;
			freeRam(used[i].second.first, used[i].second.second);
			break;
		}
		used.erase(used.begin() + index);
	}
	this->resize(this->width(), this->height() + 1);
	this->resize(this->width(), this->height() - 1);
}

void MainWindow::draw(){
	int labelHeight = ui->label_graph->height();
	auto ramSize2Width = [&] (int a) {return int(double(a) * (double(ui->label_graph->width()) / double(RAMSIZE))); };

	QPainter *painter = new QPainter;
	painter->begin(ui->label_graph);
	painter->setPen(QPen(Qt::red, 0));
	painter->setBrush(QBrush(Qt::red, Qt::SolidPattern));
	for(auto i : used) painter->drawRect(ramSize2Width(i.second.first), 0, ramSize2Width(i.second.second - i.second.first), labelHeight);
	painter->setPen(QPen(Qt::green, 0));
	painter->setBrush(QBrush(Qt::green, Qt::SolidPattern));
	for(auto i : remainRam) painter->drawRect(ramSize2Width(i.first), 0, ramSize2Width(i.second - i.first), labelHeight);
	painter->setPen(QPen(Qt::blue, 0));
	for(auto i : used) {
		painter->drawLine(ramSize2Width(i.second.first), 0, ramSize2Width(i.second.second), labelHeight);
		painter->drawLine(ramSize2Width(i.second.first), labelHeight, ramSize2Width(i.second.second), 0);
	}
	painter->end();
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event){
	if(watched == ui->label_graph && event->type() == QEvent::Paint) draw();
	return QWidget::eventFilter(watched, event);
}
