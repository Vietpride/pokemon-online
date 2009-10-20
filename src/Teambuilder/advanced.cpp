#include "advanced.h"
#include <QtGui>

TB_Advanced::TB_Advanced(PokeTeam *_poke)
{
    resize(300,400);

    m_poke = _poke;

    QHBoxLayout *layout = new QHBoxLayout(this);
    QVBoxLayout *firstColumn = new QVBoxLayout();
    QVBoxLayout *secondColumn = new QVBoxLayout();

    layout->addLayout(firstColumn,1);
    layout->addLayout(secondColumn,0);

    QGroupBox *hiddenpower = new QGroupBox(tr("Hidden Power"));
    firstColumn->addWidget(hiddenpower);
    QGridLayout *hidpower = new QGridLayout(hiddenpower);

    hpchoice = new QComboBox();
    hpower = new QLabel();
    hidpower->addWidget(new QLabel("Type:"), 0, 0);
    hidpower->addWidget(hpchoice, 0, 1);
    hidpower->addWidget(new QLabel("Power:"), 1,0);
    hidpower->addWidget(hpower,1,1);

    for (int i = 1; i < TypeInfo::NumberOfTypes() - 1; i++)
    {
	hpchoice->addItem(TypeInfo::Name(i));
    }

    QGroupBox *dvs = new QGroupBox("DVs");
    firstColumn->addWidget(dvs);
    QGridLayout *dvlayout = new QGridLayout(dvs);
    QStringList stats_l;
    stats_l << "Hit Points" << "Attack" << "Defense" << "Speed" << "Special attack" << "Special defense";

    for (int i = 0; i < 6; i++)
    {
	dvlayout->addWidget(new QLabel(stats_l[i]), i, 0);
	dvlayout->addWidget((dvchoice[i]= new QSpinBox()), i, 1);

	dvchoice[i]->setRange(0,31);
	dvchoice[i]->setAccelerated(true);
	connect(dvchoice[i], SIGNAL(valueChanged(int)), SLOT(changeDV(int)));

	QColor colors[3] = {Qt::darkBlue, Qt::black, Qt::red};
	QColor mycol = colors[poke()->natureBoost(i)+1];

	dvlayout->addWidget((stats[i]=new QLabel()), i, 2);

	QPalette pal = stats[i]->palette();
	pal.setColor(QPalette::WindowText, mycol);
	stats[i]->setPalette(pal);
    }
    updateDVs();

    stats_l.clear();
    stats_l << "HP" << "Attack" << "Defense" << "Speed" << "SpAttack" << "SpDefense";

    hpanddvchoice = new QCompactTable(5, 6);
    hpanddvchoice->verticalHeader()->hide();
    hpanddvchoice->setHorizontalHeaderLabels(stats_l);

    secondColumn->addWidget(pokeImage=new QLabel());
    updatePokeImage();

    QHBoxLayout *levellayout = new QHBoxLayout();
    secondColumn->addLayout(levellayout);
    levellayout->addWidget(new QLabel(tr("Level")));
    levellayout->addWidget(level = new QSpinBox());
    level->setRange(2,100);
    level->setValue(poke()->level());
    level->setAccelerated(true);
    connect(level, SIGNAL(valueChanged(int)), SLOT(changeLevel(int)));

    QGroupBox *gender = new QGroupBox(tr("Gender"));
    QVBoxLayout *genderLayout = new QVBoxLayout(gender);
    secondColumn->addWidget(gender);

    if (poke()->genderAvail() == Pokemon::MaleAndFemaleAvail)
    {
	genderLayout->addWidget(gender1 = new QRadioButton(tr("Male")));
	genderLayout->addWidget(gender2 = new QRadioButton(tr("Female")));
	connect(gender1, SIGNAL(toggled(bool)), SLOT(changeGender(bool)));
	updateGender();
    } else {
	genderLayout->addWidget(gender1 = new QRadioButton( poke()->gender() == Pokemon::Neutral ? tr("Neutral") : (poke()->gender() == Pokemon::Female ? tr("Female") : tr("Male"))));
	gender1->setChecked(true);
	gender1->setEnabled(false);
    }

    QGroupBox *ability = new QGroupBox(tr("Ability"));
    secondColumn->addWidget(ability);
    QVBoxLayout *abilityLayout = new QVBoxLayout(ability);

    abilityLayout->addWidget(ability1=new QRadioButton(AbilityInfo::Name(poke()->abilities()[0])));
    if (poke()->abilities()[1] != 0) {
	abilityLayout->addWidget(ability2=new QRadioButton(AbilityInfo::Name(poke()->abilities()[1])));
	connect(ability1, SIGNAL(toggled(bool)), SLOT(changeAbility(bool)));
	updateAbility();
    } else {
	ability1->setChecked(true);
	ability1->setEnabled(false);
    }

    secondColumn->addWidget(shiny = new QCheckBox(tr("Shiny")));
    if (poke()->shiny()) {
	shiny->setChecked(true);
    }
    connect(shiny, SIGNAL(toggled(bool)), SLOT(changeShininess(bool)));
}

void TB_Advanced::changeAbility(bool ab1)
{
    poke()->setAbility(ab1? poke()->abilities()[0] : poke()->abilities()[1]);
}

void TB_Advanced::changeShininess(bool shine)
{
    poke()->setShininess(shine);
    updatePokeImage();
}

void TB_Advanced::changeGender(bool gend1)
{
    poke()->setGender(gend1 ? Pokemon::Male : Pokemon::Female);
    updatePokeImage();
}

void TB_Advanced::updatePokeImage()
{
    pokeImage->setPixmap(poke()->picture());
}

void TB_Advanced::updateAbility()
{
    if (poke()->ability() == poke()->abilities()[0])
    {
	ability1->setChecked(true);
    } else {
	ability2->setChecked(true);
    }
}

void TB_Advanced::updateGender()
{
    if (poke()->genderAvail()== Pokemon::MaleAndFemaleAvail)
    {
	if (poke()->gender() == Pokemon::Male)
	{
	    gender1->setChecked(true);
	} else {
	    gender2->setChecked(true);
	}
    }
}

void TB_Advanced::changeDV(int stat, int newval)
{
    poke()->setDV(stat, newval);
    updateDV(stat);
    updateHiddenPower();
}

int TB_Advanced::stat(QObject *dvchoiceptr)
{
    for (int i = 0; i < 6; i++) {
	if (dvchoice[i] == dvchoiceptr)
	    return i;
    }

    throw tr("Fatal error in TB_Advanced::stat(QObject *) : the pointer provided does not correspond to any dvchoice");
}

void TB_Advanced::changeDV(int newval)
{
    changeDV(stat(sender()), newval);
}

void TB_Advanced::updateDVs()
{
    for (int i = 0; i < 6; i++)
    {
	updateDV(i);
    }
}

void TB_Advanced::updateDV(int stat)
{
    dvchoice[stat]->setValue(poke()->DV(stat));
    updateStat(stat);
}

void TB_Advanced::changeLevel(int level)
{
    poke()->setLevel(level);
    updateStats();
}

void TB_Advanced::updateHiddenPower()
{
}

void TB_Advanced::updateStats()
{
    for (int i = 0; i < 6; i++)
	updateStat(i);
}

void TB_Advanced::updateStat(int stat)
{
    stats[stat]->setText(QString::number(poke()->stat(stat)));
}

PokeTeam *TB_Advanced::poke()
{
    return m_poke;
}
