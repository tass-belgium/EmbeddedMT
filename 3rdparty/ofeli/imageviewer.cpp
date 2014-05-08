/****************************************************************************
**
** Copyright (C) 2010-2012 Fabien Bessy.
** Contact: fabien.bessy@gmail.com
**
** This file is part of project Ofeli.
**
** http://www.cecill.info/licences/Licence_CeCILL_V2-en.html
** You may use this file under the terms of the CeCILL license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Fabien Bessy and its Subsidiary(-ies) nor the
**     names of its contributors may be used to endorse or promote products
**     derived from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
**
****************************************************************************/

// origin code from Qt exemple "imageviewer"
// http://doc.qt.nokia.com/4.7/widgets-imageviewer.html

////////////////////////// Ofeli /////////////////////////////

// Qt GUI part of Ofeli

#include "imageviewer.hpp"
#include "pixmapwidget.hpp"

                    /////////////////////

// image processing part of Ofeli

#include "ac_withoutedges.hpp"
#include "ac_withoutedges_yuv.hpp"
#include "geodesic_ac.hpp"
#include "filters.hpp"   // dependent of Boost library
#include "hausdorff_distance.hpp"

//////////////////////////////////////////////////////////////

#include <QtWidgets>
#ifndef QT_NO_PRINTER
#include <QPrintDialog>
#endif

#include <ctime>         // std::clock_t and std::clock()
#include <cstdlib>       // std::memcpy
// if you want a partial support of DICOM image, without support of JPEG-2000 encapsulated image
// install DCMTK library to the project
// uncomment row 72, 1002 and the block at 1074.
//#include "dcmtk/dcmimgle/dcmimage.h"

namespace ofeli
{

// Constructeur
ImageViewer::ImageViewer()
{
/*
    list<float> l(500);
    l.push_front(5);
    l.push_front(15);
    l.push_front(-5);
    l.push_front(54);
    l.push_front(-5);
    l.push_front(13.9);
    l.push_front(14);
    l.push_front(15);
    l.push_front(2);
    l.push_front(0);
    l.push_front(-8);

    l.sort();

    list<float> ll(1000);
    ll.push_front(59);
    ll.push_front(125);

    l.display();
    ll.display();

    l.display();
    ll.display();*/
    /*ll.push_front(-59);
    ll.push_front(59);
    ll.push_front(-53);
    ll.push_front(13.29);
    ll.push_front(14);
    ll.push_front(15);
    ll.push_front(20);
    ll.push_front(0);
    ll.push_front(-82);*/

    //l.display();
    //ll.display();
    //l.splice_front(ll);

   // ll.display();

   // l = ll;
    //ll.clear();

    //l.display();
    //ll.display();
    //l.splice_front(ll);

    //ll.display();
    //ll.display();

    //l.sort(greater<float>());
    //l.display();
    /*l.sort();
    l.display();
    l.sort(greater<float>());
    l.display();
    l.remove_if(predicate_example<float>());
    l.display();
    l.put_away(ll);
    l.display();*/

    /////////// initialisation fenêtre principale /////////////////////

    // titre de la fenêtre principale
    setWindowTitle(tr("Ofeli"));

    if( operating_system == 3 )
    {
        setWindowIcon(QIcon(":Ofeli.png"));
    }

    //resize(600,375);
    QSettings settings("Bessy", "Ofeli");

    resize(settings.value("mainWindow/size", QSize(600, 375)).toSize());
    move(settings.value("mainWindow/position", QPoint(200, 200)).toPoint());

    last_directory_used = settings.value("name/last_directory_used",QDir().homePath()).toString();

    // créations des actions et menus
    createActions();
    createMenus();

    // widget permettant d'afficher l'image résultat
    scrollArea = new QScrollArea;
    imageLabel = new ofeli::PixmapWidget(scrollArea);
    imageLabel->setBackgroundRole(QPalette::Dark);
    imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    imageLabel->setScaledContents(true);
    imageLabel->setFocusPolicy(Qt::StrongFocus);
    // pour avoir la position du curseur de la souris au survol
    imageLabel->setMouseTracking(true);
    imageLabel->installEventFilter(this);
    imageLabel->setAcceptDrops(true);
    imageLabel->resize(280, 487);
    imageLabel->setAlignment(Qt::AlignCenter);
    imageLabel->set_text(tr("<drag image>"));

    // widget contenant le widget précédant et offrant des scrolls barres a l'image
    scrollArea->setBackgroundRole(QPalette::Dark);
    scrollArea->setWidget(imageLabel);
    scrollArea->setWidgetResizable(true);
    scrollArea->setAlignment(Qt::AlignCenter);
    scrollArea->installEventFilter(this);
    connect(scrollArea->verticalScrollBar(),SIGNAL(rangeChanged(int,int)),this,SLOT(adjustVerticalScroll (int,int)));
    connect(scrollArea->horizontalScrollBar(),SIGNAL(rangeChanged(int,int)),this,SLOT(adjustHorizontalScroll(int,int)));


    // widget qui va contenir du texte avec la postion et la valeur du pixel survolé par le curseur
    pixel_text = new QLabel(this);
    pixel_text->setMinimumWidth(248);
    pixel_text->setText(tr("img(x,y) ="));
    pixel_text->setToolTip(tr("pixel value at the position (x,y)"));
    phi_text = new QLabel(this);
    phi_text->setText("ϕ(x,y,t) =");
    phi_text->setToolTip(tr("level set function value at the position (x,y,t), value ∈ { -3, -1, 1, 3 }"));
    lists_text = new QLabel(this);
    lists_text->setText("Lin(t) & Lout(t)");

    // widget qui va contenir du texte avec le nombre d'itération et le nombre de valeur de phi qui ont changé
    text = new QLabel(this);
    text->setText(tr("iteration t ="));
    text->setToolTip(tr("must be strictly less than iteration_max = 5 × max(img_width,img_height)"));
    changes_text = new QLabel(this);
    changes_text->setText(tr("lists changes ="));
    changes_text->setToolTip(tr("updated only for each iteration of the cycle 1"));
    oscillation_text = new QLabel(this);
    oscillation_text->setText(tr("oscillation ="));
    oscillation_text->setToolTip(tr("updated only at the end of the cycle 2"));
    // widget qui va contenir du texte avec la moyenne extérieure et intérieure ou le seuil Otsu du gradient en fonction du modèle de vitesse choisi
    Cin_text = new QLabel(this);
    Cin_text->setText("Cin =");
    Cout_text = new QLabel(this);
    Cout_text->setText("Cout =");
    Cout_text->setToolTip(tr("outside average of the image"));
    Cin_text->setToolTip(tr("inside average of the image"));
    threshold_text = new QLabel(this);
    threshold_text->setToolTip(tr("0.7 times this threshold before to begin the steepest descent"));
    // widget qui va contenir du texte avec le temps écoulé
    time_text = new QLabel(this);
    time_text->setText(tr("time ="));
    time_text->setToolTip(tr("elapsed time in the loop, without the initialization time when the constructor is called"));

    // permet de centrer horizontalement les widgets texte
    lists_text->setAlignment(Qt::AlignRight);
    pixel_text->setAlignment(Qt::AlignRight);
    phi_text->setAlignment(Qt::AlignRight);
    text->setAlignment(Qt::AlignRight);
    changes_text->setAlignment(Qt::AlignRight);
    oscillation_text->setAlignment(Qt::AlignRight);
    Cin_text->setAlignment(Qt::AlignRight);
    Cout_text->setAlignment(Qt::AlignRight);
    threshold_text->setAlignment(Qt::AlignRight);
    time_text->setAlignment(Qt::AlignRight);


    QGroupBox* speed1_group = new QGroupBox(tr("Chan-Vese model information"));
    QVBoxLayout* chanvese_text = new QVBoxLayout;
    chanvese_text->addWidget(Cout_text);
    chanvese_text->addWidget(Cin_text);
    speed1_group->setLayout(chanvese_text);
    QGroupBox* speed2_group = new QGroupBox(tr("Geodesic model information"));
    QVBoxLayout* threshold_layout = new QVBoxLayout;
    threshold_layout->addWidget(threshold_text);
    speed2_group->setLayout(threshold_layout);

    // widget stack qui va afficher le bon widget en fonction du modèle de vitesse sélectionné
    stackedWidget = new QStackedWidget;
    stackedWidget->addWidget(speed1_group);
    stackedWidget->addWidget(speed2_group);

    QGroupBox* stop_group = new QGroupBox(tr("Stopping conditions"));
    QVBoxLayout* condi_layout = new QVBoxLayout;
    condi_layout->addWidget(text);
    condi_layout->addWidget(changes_text);
    condi_layout->addWidget(oscillation_text);
    stop_group->setLayout(condi_layout);

    QGroupBox* time_group = new QGroupBox(tr("Elapsed time"));
    QVBoxLayout* elapsed_layout = new QVBoxLayout;
    elapsed_layout->addWidget(time_text);
    time_group->setLayout(elapsed_layout);

    QGroupBox* data_group = new QGroupBox(tr("Data, level set function and lists"));
    QVBoxLayout* data_layout = new QVBoxLayout;
    data_layout->addWidget(pixel_text);
    data_layout->addWidget(phi_text);
    data_layout->addWidget(lists_text);
    data_group->setLayout(data_layout);

    scale_spin0 = new QSpinBox;
    scale_spin0->setSingleStep(25);
    scale_spin0->setMinimum(1);
    scale_spin0->setMaximum(5000);
    scale_spin0->setSuffix(" %");

    scale_slider0 = new QSlider(Qt::Horizontal, this);
    scale_slider0->setTickPosition(QSlider::TicksAbove);
    scale_slider0->setMinimum(1);
    scale_slider0->setMaximum(1000);
    scale_slider0->setTickInterval(100);
    scale_slider0->setSingleStep(25);
    scale_slider0->setValue(100);

    QFormLayout* scale_spin_layout = new QFormLayout;
    scale_spin_layout->addRow(tr("scale :"), scale_spin0);
    scale_spin_layout->setFormAlignment(Qt::AlignRight);

    QVBoxLayout* scale_layout = new QVBoxLayout;
    scale_layout->addLayout(scale_spin_layout);
    scale_layout->addWidget(scale_slider0);

    QGroupBox* scale_group = new QGroupBox(tr("Display"));
    scale_group->setLayout(scale_layout);
    scale_spin0->installEventFilter(this);
    scale_slider0->installEventFilter(this);
    scale_spin0->setMouseTracking(true);
    scale_slider0->setMouseTracking(true);

    connect(scale_spin0,SIGNAL(valueChanged(int)),this,SLOT(do_scale0(int)));
    connect(scale_slider0,SIGNAL(valueChanged(int)),scale_spin0,SLOT(setValue(int)));
    connect(scale_spin0,SIGNAL(valueChanged(int)),scale_slider0,SLOT(setValue(int)));

    // Layout vertical réunissant les groupbox précédents
    QVBoxLayout* info_layout = new QVBoxLayout;
    info_layout->addWidget(stackedWidget);
    info_layout->addWidget(stop_group);
    info_layout->addWidget(time_group);
    info_layout->addWidget(data_group);
    info_layout->addWidget(scale_group);
    info_layout->addStretch(1);

    // Gridlayout réunissant la scrollArea contenant l'image à gauche et les informations à droite.
    QGridLayout* main_layout = new QGridLayout;

    main_layout->addWidget(scrollArea, 0, 0);
    main_layout->addLayout(info_layout, 0, 1);

    // permet de ne pas avoir d'étirement lié au resize
    main_layout->setColumnStretch ( 0, 1 );

    // permet de mettre une limite minimale au resize de la fenêtre en fonction du layout (de la taille des widgets texte)
    main_layout->setSizeConstraint(QLayout::SetMinimumSize);

    // création du widget principal à partir du layout grid
    QLabel* main_widget = new QLabel(this);
    main_widget->setLayout(main_layout);

    // widget de la fenêtre principale de l'application
    setCentralWidget(main_widget);


    /////////// initialisation fenêtre de configuration /////////////////////


    // création de la fenêtre de configuration
    settings_window = new QDialog(this);

    // titre pour la fenêtre de configuration
    settings_window->setWindowTitle(tr("Settings"));

    // widget permettant d'afficher l'image montrant les modifications des réglages
    scrollArea_settings = new QScrollArea;
    imageLabel_settings = new ofeli::PixmapWidget(scrollArea_settings);
    imageLabel_settings->setBackgroundRole(QPalette::Dark);
    imageLabel_settings->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    imageLabel_settings->setScaledContents(true);

    imageLabel_settings->setMouseTracking(true);
    imageLabel_settings->installEventFilter(this);
    imageLabel_settings->setAcceptDrops(true);
    imageLabel_settings->setAlignment(Qt::AlignCenter);
    QString text(tr("<drag ϕ(t=0)>"));
    imageLabel_settings->set_text(text);
    imageLabel_settings->resize(200,200);

    // widget contenant le widget précédant et offrant des scrolls barres à l'image
    scrollArea_settings->setBackgroundRole(QPalette::Dark);
    scrollArea_settings->setWidget(imageLabel_settings);
    scrollArea_settings->setAlignment(Qt::AlignCenter);
    scrollArea_settings->setWidgetResizable(true);

    connect(scrollArea_settings->verticalScrollBar(),SIGNAL(rangeChanged(int,int)),this,SLOT(adjustVerticalScroll_settings (int,int)));
    connect(scrollArea_settings->horizontalScrollBar(),SIGNAL(rangeChanged(int,int)),this,SLOT(adjustHorizontalScroll_settings(int,int)));

    // widget pour avoir les bouttons Ok Cancel, standardisé et organisé dans un layout
    dial_buttons = new QDialogButtonBox(settings_window);

    // ajoute boutton Ok et Cancel au widget
    dial_buttons->addButton(QDialogButtonBox::Ok);
    dial_buttons->addButton(QDialogButtonBox::Cancel);
    dial_buttons->addButton(QDialogButtonBox::Reset);

    // connecte le widget dial_buttons à la fenêtre de configuration
    // Cliquer sur le boutton Ok dans cette fenêtre emet un signet accepted() qui est reçu par la fenêtre de configuration.
    // Cette fenêtre modale va se fermer et renvoyer la valeur true pour le boolléen result_settings_window. Les changements de paramètres seront effectués.
    connect( dial_buttons, SIGNAL(accepted()), settings_window, SLOT(accept()) );
    // Cliquer sur le boutton Cancel dans cette fenêtre emet un signet rejected() qui est reçu par la fenêtre de configuration.
    // Cette fenêtre modale va se fermer et renvoyer la valeur false pour le boolléen result_settings_window. Les changements de paramètres ne seront pas effectués.
    connect( dial_buttons, SIGNAL(rejected()), settings_window, SLOT(reject()) );

    connect( dial_buttons->button(QDialogButtonBox::Reset), SIGNAL(clicked()), this, SLOT(default_settings()) );


    // initilisation des variables de classe, pointeurs image (pointeur sur octet)
    // la mémoire est désallouée (sauf à la première ouverture, d'où l'interet d'initialiser à NULL)
    // puis allouée à chaque open() appelé d'une image
    img1 = NULL;

    // initialisation de tous les pointeurs servant à la fenêtre de configuration
    phi_init1 = NULL;
    phi_init2 = NULL;
    phi_init1_clean = NULL;
    phi_init2_clean = NULL;
    shape = NULL;
    shape_points = NULL;
    Lout_shape1 = NULL;
    Lin_shape1 = NULL;
    Lout_2 = NULL;
    Lin_2 = NULL;
    filters1 = NULL;
    filters2 = NULL;

    listA = NULL;
    listB = NULL;

    image_result_uchar = NULL;
    image_uchar = NULL;
    image_filter_uchar = NULL;
    image_phi_uchar = NULL;
    image_shape_uchar = NULL;

    ac = NULL;

    // création du widget onglets, visible à gauche dans la fenêtre de configuration
    onglets = new QTabWidget(settings_window);

    // création de widgets correspondant à chaque page ou à chaque onglet du QTabWidget onglets
    QWidget* page1 = new QWidget;
    QWidget* page2 = new QWidget;
    // page 3 va être crée comme QTabWidget et la fonction preprocessing()
    QWidget* page4 = new QWidget;

    // widget contenant des layouts
    page1->setLayout(algorithm());
    page2->setLayout(initialization());
    page4->setLayout(display());

    // ajout des 4 widgets correspondant aux 4 onglets (aux 4 pages) dans le QTabWidget avec un titre pour chaque onglet
    onglets->addTab(page1, tr("Algorithm"));
    onglets->addTab(page2, tr("Initialization"));
    onglets->addTab(preprocessing(), tr("Preprocessing"));
    onglets->addTab(page4, tr("Display"));

    // création d'un layout grid (organisation en grille des widgets) pour la fenêtre de configuration
    QGridLayout *settings_grid = new QGridLayout;

    // ajout en haut à gauche des onglets dans le layout de la fenêtre de configuration
    settings_grid->addWidget(onglets, 0, 0);
    // ajout en haut à droite de scrollArea_settings (widget contenant l'image affichant les modifications) dans le layout de la fenêtre de configuration
    settings_grid->addWidget(scrollArea_settings, 0, 1);
    // ajout en bas à droite des bouttons Ok Cancel dans le layout de la fenêtre de configuration
    settings_grid->addWidget(dial_buttons,1,1);

    // permet de ne pas avoir d'étirement lié au resize pour les onglets
    settings_grid->setColumnStretch ( 1, 1 );
    // layout donné à la fenêtre de configuration
    settings_window->setLayout(settings_grid);

    // signal émis au changement d'onglet pour montrer dans l'image phi ou l'ellipse+phi pour l'onglet initialization
    connect(onglets,SIGNAL(currentChanged(int)),this,SLOT(tab_visu(int)));

    // resize de la fenêtre configuration pour s'ajuster parfaitement à l'image 550*550
    switch( operating_system )
    {
    case 1 :
    {
        settings_window->resize(settings.value("settingsWindow/size", QSize(550+335,550+55)).toSize());
        settings_window->move(settings.value("settingsWindow/position", QPoint(200, 200)).toPoint());
        break;
    }
    case 2 :
    {
        settings_window->resize(settings.value("settingsWindow/size", QSize(550+477,550+82)).toSize());
        settings_window->move(settings.value("settingsWindow/position", QPoint(200, 200)).toPoint());
        break;
    }
    case 3 :
    {
        settings_window->resize(settings.value("settingsWindow/size", QSize(550+327,550+60)).toSize());
        settings_window->move(settings.value("settingsWindow/position", QPoint(200, 200)).toPoint());
    }
    }

    // Paramètres pour la fenêtre de configuration


    ///////////////////////////////////////////////

    // paramètres par défaut de l'onglet Algorithm

    ///////////////////////////////////////////////

    Na1 = settings.value("Settings/Algorithm/Na", 30).toInt();
    Ns1 = settings.value("Settings/Algorithm/Ns", 3).toInt();
    model = settings.value("Settings/Algorithm/model", 1).toInt();
    isRgb1 = settings.value("Settings/Algorithm/isRgb", true).toBool();
    lambda_out1 = settings.value("Settings/Algorithm/lambda_out", 1).toInt();
    lambda_in1 = settings.value("Settings/Algorithm/lambda_in", 1).toInt();
    kernel_gradient_length1 = settings.value("Settings/Algorithm/kernel_gradient_length", 5).toInt();
    alpha1 = settings.value("Settings/Algorithm/alpha", 1).toInt();
    beta1 = settings.value("Settings/Algorithm/beta", 10).toInt();
    gamma1 = settings.value("Settings/Algorithm/gamma", 10).toInt();
    hasSmoothingCycle1 = settings.value("Settings/Algorithm/hasSmoothingCycle", true).toBool();
    kernel_curve1 = settings.value("Settings/Algorithm/kernel_curve", 5).toInt();
    std_curve1 = settings.value("Settings/Algorithm/std_curve", 2.0).toDouble();

    //////////////////////////////////////////////////

    ///////////////////////////////////////////////////

    // paramètres par défaut de l'onglet Initialisation

    ////////////////////////////////////////////////////

    hasEllipse1 = settings.value("Settings/Initialization/hasEllipse", true).toBool();
    init_width1 = settings.value("Settings/Initialization/init_width", 0.65).toDouble();
    init_height1 = settings.value("Settings/Initialization/init_height", 0.65).toDouble();
    center_x1 = settings.value("Settings/Initialization/center_x", 0.0).toDouble();
    center_y1 = settings.value("Settings/Initialization/center_y", 0.0).toDouble();

    ////////////////////////////////////////////////////

    ///////////////////////////////////////////////////

    // paramètres par défaut de l'onglet Preprocessing

    ////////////////////////////////////////////////////

    // variable permertant de cacher ou faire apparaitre le contour au clic droit
    hasContoursHidden = true;

    hasPreprocess = settings.value("Settings/Preprocessing/hasPreprocess", false).toBool();
    hasGaussianNoise = settings.value("Settings/Preprocessing/hasGaussianNoise", false).toBool();
    std_noise = settings.value("Settings/Preprocessing/std_noise", 20.0).toDouble();
    hasSaltNoise = settings.value("Settings/Preprocessing/hasSaltNoise", false).toBool();
    proba_noise = settings.value("Settings/Preprocessing/proba_noise", 0.05).toDouble();
    hasSpeckleNoise = settings.value("Settings/Preprocessing/hasSpeckleNoise", false).toBool();
    std_speckle_noise = settings.value("Settings/Preprocessing/std_speckle_noise", 0.16).toDouble();

    hasMedianFilt = settings.value("Settings/Preprocessing/hasMedianFilt", false).toBool();
    kernel_median_length1 = settings.value("Settings/Preprocessing/kernel_median_length", 5).toInt();
    hasO1algo1 = settings.value("Settings/Preprocessing/hasO1algo", true).toBool();
    hasMeanFilt = settings.value("Settings/Preprocessing/hasMeanFilt", false).toBool();
    kernel_mean_length1 = settings.value("Settings/Preprocessing/kernel_mean_length", 5).toInt();
    hasGaussianFilt = settings.value("Settings/Preprocessing/hasGaussianFilt", false).toBool();
    kernel_gaussian_length1 = settings.value("Settings/Preprocessing/kernel_gaussian_length", 5).toInt();
    sigma = settings.value("Settings/Preprocessing/sigma", 2.0).toDouble();

    hasAnisoDiff = settings.value("Settings/Preprocessing/hasAnisoDiff", false).toBool();
    aniso_option1 = settings.value("Settings/Preprocessing/aniso_option", 1).toInt();
    max_itera1 = settings.value("Settings/Preprocessing/max_itera", 10).toInt();
    lambda1 = settings.value("Settings/Preprocessing/lambda", 1.0/7.0).toDouble();
    kappa1 = settings.value("Settings/Preprocessing/kappa", 30.0).toDouble();

    hasOpenFilt = settings.value("Settings/Preprocessing/hasOpenFilt", false).toBool();
    kernel_open_length1 = settings.value("Settings/Preprocessing/kernel_open_length", 5).toInt();

    hasCloseFilt = settings.value("Settings/Preprocessing/hasCloseFilt", false).toBool();
    kernel_close_length1 = settings.value("Settings/Preprocessing/kernel_close_length", 5).toInt();

    hasTophatFilt = settings.value("Settings/Preprocessing/hasTophatFilt", false).toBool();
    isWhiteTophat = settings.value("Settings/Preprocessing/isWhiteTophat", true).toBool();
    kernel_tophat_length1 = settings.value("Settings/Preprocessing/kernel_tophat_length", 5).toInt();

    hasO1morpho1 = settings.value("Settings/Preprocessing/hasO1morpho", true).toBool();

    ////////////////////////////////////////////

    // paramètres par défaut de l'onglet Display

    ///////////////////////////////////////////

    scale_spin0->setValue(settings.value("Settings/Display/zoom_factor", 100).toInt());
    hasHistoNormaliz = settings.value("Settings/Display/hasHistoNormaliz", true).toBool();

    hasDisplayEach = settings.value("Settings/Display/hasDisplayEach", true).toBool();

    outside_combo = settings.value("Settings/Display/outside_combo", 2).toInt();
    inside_combo = settings.value("Settings/Display/inside_combo", 0).toInt();

    Rout_selected1 = (unsigned char)(settings.value("Settings/Display/Rout_selected", 128).toInt());
    Gout_selected1 = (unsigned char)(settings.value("Settings/Display/Gout_selected", 0).toInt());
    Bout_selected1 = (unsigned char)(settings.value("Settings/Display/Bout_selected", 255).toInt());
    Rin_selected1 = (unsigned char)(settings.value("Settings/Display/Rin_selected", 255).toInt());
    Gin_selected1 = (unsigned char)(settings.value("Settings/Display/Gin_selected", 128).toInt());
    Bin_selected1 = (unsigned char)(settings.value("Settings/Display/Bin_selected", 0).toInt());

    seconds_wait = settings.value("Settings/Display/seconds_wait", 0.0).toDouble();

    /////////////////////////////////////////////////

    imageLabel->set_zoomFactor(float(scale_spin0->value())/100.0f);
    imageLabel_settings->set_zoomFactor(float(scale_spin0->value())/100.0f);

    // fonction permettant de régler les widgets par défaut (de les initialiser), radioboutton, checkbox, spinbox, ect... dans la fenêtre de configuration par rapport aux valeurs précédentes des paramètres de configuration
    cancel_settings();
    scrollArea_settings->setFocus(Qt::OtherFocusReason);

    /////////// initialisation fenêtre d'evaluation /////////////////////

    // création de la fenêtre de configuration
    evaluation_window = new QDialog(this,Qt::WindowMinimizeButtonHint);

    // titre pour la fenêtre de configuration
    evaluation_window->setWindowTitle(tr("Evaluation"));

    evaluation_window->move(settings.value("evaluationWindow/position", QPoint(200, 200)).toPoint());
    evaluation_window->resize(settings.value("evaluationWindow/size", QSize(665,542)).toSize());

    // widget permettant d'afficher l'image montrant les modifications des réglages
    scrollAreaA = new QScrollArea;
    imageLabelA = new ofeli::PixmapWidget(scrollAreaA);
    imageLabelA->setBackgroundRole(QPalette::Dark);
    imageLabelA->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    imageLabelA->setScaledContents(true);
    imageLabelA->setMouseTracking(true);
    imageLabelA->installEventFilter(this);
    imageLabelA->setAcceptDrops(true);
    imageLabelA->resize(200, 200);
    imageLabelA->setAlignment(Qt::AlignCenter);
    imageLabelA->set_text(tr("<drag image>"));

    // widget contenant le widget précédant et offrant des scrolls barres a l'image
    scrollAreaA->setBackgroundRole(QPalette::Dark);
    scrollAreaA->setWidget(imageLabelA);
    scrollAreaA->setAlignment(Qt::AlignCenter);
    scrollAreaA->setWidgetResizable(true);


    // widget permettant d'afficher l'image montrant les modifications des réglages
    scrollAreaB = new QScrollArea;
    imageLabelB = new ofeli::PixmapWidget(scrollAreaB);
    imageLabelB->setBackgroundRole(QPalette::Dark);
    imageLabelB->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    imageLabelB->setScaledContents(true);
    imageLabelB->setMouseTracking(true);
    imageLabelB->installEventFilter(this);
    imageLabelB->setAcceptDrops(true);
    imageLabelB->resize(200, 200);
    imageLabelB->setAlignment(Qt::AlignCenter);
    imageLabelB->set_text(tr("<drag image>"));

    // widget contenant le widget précédant et offrant des scrolls barres a l'image
    scrollAreaB->setBackgroundRole(QPalette::Dark);
    scrollAreaB->setWidget(imageLabelB);
    scrollAreaB->setAlignment(Qt::AlignCenter);
    scrollAreaB->setWidgetResizable(true);

    connect(scrollAreaA->verticalScrollBar(),SIGNAL(rangeChanged(int,int)),this,SLOT(adjustVerticalScrollA (int,int)));
    connect(scrollAreaA->horizontalScrollBar(),SIGNAL(rangeChanged(int,int)),this,SLOT(adjustHorizontalScrollA(int,int)));

    connect(scrollAreaB->verticalScrollBar(),SIGNAL(rangeChanged(int,int)),this,SLOT(adjustVerticalScrollB (int,int)));
    connect(scrollAreaB->horizontalScrollBar(),SIGNAL(rangeChanged(int,int)),this,SLOT(adjustHorizontalScrollB(int,int)));

    QPushButton* open1_button = new QPushButton(tr("Open image"));
    connect(open1_button,SIGNAL(clicked()),this,SLOT(openFileNameA()));

    QPushButton* open2_button = new QPushButton(tr("Open image"));
    connect(open2_button,SIGNAL(clicked()),this,SLOT(openFileNameB()));

    scale_spinA = new QSpinBox;
    scale_spinA->setSingleStep(25);
    scale_spinA->setMinimum(1);
    scale_spinA->setMaximum(5000);
    scale_spinA->setSuffix(" %");
    scale_spinA->setValue(100);
    connect(scale_spinA,SIGNAL(valueChanged(int)),this,SLOT(scaleA(int)));

    scale_spinB = new QSpinBox;
    scale_spinB->setSingleStep(25);
    scale_spinB->setMinimum(1);
    scale_spinB->setMaximum(5000);
    scale_spinB->setSuffix(" %");
    scale_spinB->setValue(100);
    connect(scale_spinB,SIGNAL(valueChanged(int)),this,SLOT(scaleB(int)));

    scale_spinA->installEventFilter(this);
    scale_spinB->installEventFilter(this);
    scale_spinA->setMouseTracking(true);
    scale_spinB->setMouseTracking(true);

    QFormLayout* scaleA = new QFormLayout;
    scaleA->addRow(tr("Display scale :"),scale_spinA);

    QFormLayout* scaleB = new QFormLayout;
    scaleB->addRow(tr("Display scale :"),scale_spinB);

    label_name1 = new QLabel(this);
    label_name2 = new QLabel(this);
    label_hausdorff = new QLabel(this);
    label_modified_hausdorff = new QLabel(this);
    label_hausdorff_ratio = new QLabel(this);
    label_modified_hausdorff_ratio = new QLabel(this);
    label_centroids_dist = new QLabel(this);
    label_centroids_ratio = new QLabel(this);

    // affichage
    label_hausdorff->setText(tr("Hausdorff distance = "));
    label_hausdorff_ratio->setText(tr("Hausdorff ratio = "));
    label_modified_hausdorff->setText(tr("modified Hausdorff distance = "));
    label_modified_hausdorff_ratio->setText(tr("modified Hausdorff ratio = "));
    label_centroids_dist->setText(tr("distance between centroids = "));
    label_centroids_ratio->setText(tr("ratio between centroids = "));

    label_name1->setText(tr("Title - Size"));
    label_name2->setText(tr("Title - Size"));

    label_name1->setAlignment(Qt::AlignCenter);
    label_name2->setAlignment(Qt::AlignCenter);
    label_hausdorff->setAlignment(Qt::AlignCenter);
    label_modified_hausdorff->setAlignment(Qt::AlignCenter);
    label_hausdorff_ratio->setAlignment(Qt::AlignCenter);
    label_modified_hausdorff_ratio->setAlignment(Qt::AlignCenter);
    label_centroids_dist->setAlignment(Qt::AlignCenter);
    label_centroids_ratio->setAlignment(Qt::AlignCenter);

    QVBoxLayout* imageA_layout = new QVBoxLayout;
    imageA_layout->addWidget(label_name1);
    imageA_layout->addWidget(scrollAreaA);
    imageA_layout->addLayout(scaleA);
    imageA_layout->addWidget(open1_button);

    QVBoxLayout* imageB_layout = new QVBoxLayout;
    imageB_layout->addWidget(label_name2);
    imageB_layout->addWidget(scrollAreaB);
    imageB_layout->addLayout(scaleB);
    imageB_layout->addWidget(open2_button);

    QVBoxLayout* hausdorff_layout = new QVBoxLayout;
    hausdorff_layout->addWidget(label_hausdorff);
    hausdorff_layout->addWidget(label_hausdorff_ratio);

    QVBoxLayout* modified_haus_layout = new QVBoxLayout;
    modified_haus_layout->addWidget(label_modified_hausdorff);
    modified_haus_layout->addWidget(label_modified_hausdorff_ratio);

    QVBoxLayout* centroids_layout = new QVBoxLayout;
    centroids_layout->addWidget(label_centroids_dist);
    centroids_layout->addWidget(label_centroids_ratio);

    Acolor_combobox = new QComboBox();
    Bcolor_combobox = new QComboBox();
    connect(Acolor_combobox,SIGNAL(currentIndexChanged(int)),this,SLOT(create_listA(int)));
    connect(Bcolor_combobox,SIGNAL(currentIndexChanged(int)),this,SLOT(create_listB(int)));

    // QPixmap pm : petite image affichant la couleur devant le nom de la couleur dans le combobox
    QPixmap pm(12,12);

    pm.fill(Qt::red);
    Acolor_combobox->addItem (pm, tr("Red"));
    Bcolor_combobox->addItem (pm, tr("Red"));
    pm.fill(Qt::green);
    Acolor_combobox->addItem (pm, tr("Green"));
    Bcolor_combobox->addItem (pm, tr("Green"));
    pm.fill(Qt::blue);
    Acolor_combobox->addItem (pm, tr("Blue"));
    Bcolor_combobox->addItem (pm, tr("Blue"));
    pm.fill(Qt::cyan);
    Acolor_combobox->addItem (pm, tr("Cyan"));
    Bcolor_combobox->addItem (pm, tr("Cyan"));
    pm.fill(Qt::magenta);
    Acolor_combobox->addItem (pm, tr("Magenta"));
    Bcolor_combobox->addItem (pm, tr("Magenta"));
    pm.fill(Qt::yellow);
    Acolor_combobox->addItem (pm, tr("Yellow"));
    Bcolor_combobox->addItem (pm, tr("Yellow"));
    pm.fill(Qt::black);
    Acolor_combobox->addItem (pm, tr("Black"));
    Bcolor_combobox->addItem (pm, tr("Black"));
    pm.fill(Qt::white);
    Acolor_combobox->addItem (pm, tr("White"));
    Bcolor_combobox->addItem (pm, tr("White"));

    redAselected = (unsigned char)(settings.value("Evaluation/Ra", 128).toInt());
    greenAselected = (unsigned char)(settings.value("Evaluation/Ga", 0).toInt());
    blueAselected = (unsigned char)(settings.value("Evaluation/Ba", 255).toInt());
    redBselected = (unsigned char)(settings.value("Evaluation/Rb", 255).toInt());
    greenBselected = (unsigned char)(settings.value("Evaluation/Gb", 128).toInt());
    blueBselected = (unsigned char)(settings.value("Evaluation/Bb", 0).toInt());
    pm.fill(QColor(redAselected, greenAselected, blueAselected));
    Acolor_combobox->addItem (pm, tr("Selected"));
    pm.fill(QColor(redBselected, greenBselected, blueBselected));
    Bcolor_combobox->addItem (pm, tr("Selected"));

    QPushButton* colorA_select = new QPushButton(tr("Select"));
    connect(colorA_select,SIGNAL(clicked()),this,SLOT(colorA()));

    QPushButton* colorB_select = new QPushButton(tr("Select"));
    connect(colorB_select,SIGNAL(clicked()),this,SLOT(colorB()));


    QGroupBox* hausdorff_group = new QGroupBox(tr("Hausdorff measure"));
    hausdorff_group->setLayout(hausdorff_layout);
    QGroupBox* modified_haus_group = new QGroupBox(tr("Modified Hausdoff measure"));
    modified_haus_group->setLayout(modified_haus_layout);
    QGroupBox* centroids_group = new QGroupBox(tr("Shapes gap"));
    centroids_group->setLayout(centroids_layout);

    QGroupBox* imageA_group = new QGroupBox(tr("Image A"));
    imageA_group->setLayout(imageA_layout);

    QGroupBox* imageB_group = new QGroupBox(tr("Image B"));
    imageB_group->setLayout(imageB_layout);

    QVBoxLayout* distances_layout = new QVBoxLayout;
    distances_layout->addWidget(hausdorff_group);
    distances_layout->addWidget(modified_haus_group);
    distances_layout->addWidget(centroids_group);
    distances_layout->addStretch(1);

    QFormLayout* formA = new QFormLayout;
    formA->addRow(tr("List from :"),Acolor_combobox);
    formA->addRow(tr("<click on image> |"), colorA_select);

    QFormLayout* formB = new QFormLayout;
    formB->addRow(tr("List from :"),Bcolor_combobox);
    formB->addRow(tr("<click on image> |"), colorB_select);

    QGroupBox* colorA_group = new QGroupBox(tr("Color A"));
    colorA_group->setLayout(formA);

    QGroupBox* colorB_group = new QGroupBox(tr("Color B"));
    colorB_group->setLayout(formB);

    text_listA_length = new QLabel(this);
    text_listB_length = new QLabel(this);
    text_listA_length->setAlignment(Qt::AlignCenter);
    text_listB_length->setAlignment(Qt::AlignCenter);

    text_listA_length->setText("<font color=red>"+tr("List A length = 0"));
    text_listB_length->setText("<font color=red>"+tr("List B length = 0"));

    QVBoxLayout* left_layout = new QVBoxLayout;
    left_layout->addWidget(text_listA_length);
    left_layout->addWidget(imageA_group);
    left_layout->addWidget(colorA_group);

    QVBoxLayout* right_layout = new QVBoxLayout;
    right_layout->addWidget(text_listB_length);
    right_layout->addWidget(imageB_group);
    right_layout->addWidget(colorB_group);

    QHBoxLayout* lists_select_layout = new QHBoxLayout;
    lists_select_layout->addLayout(left_layout);
    lists_select_layout->addLayout(right_layout);

    // création de widgets correspondant à chaque page ou à chaque onglet du QTabWidget onglets
    QWidget* page111 = new QWidget;
    QWidget* page222 = new QWidget;

    // widget contenant des layouts
    page111->setLayout(lists_select_layout);
    page222->setLayout(distances_layout);

    onglets_eval = new QTabWidget(evaluation_window);

    // ajout des 4 widgets correspondant aux 4 onglets (aux 4 pages) dans le QTabWidget avec un titre pour chaque onglet
    onglets_eval->addTab(page111, tr("Lists selection"));
    onglets_eval->addTab(page222, tr("Distances result"));
    onglets_eval->setTabEnabled(1,false);
    listA_length = 0;
    listB_length = 0;

    QGridLayout* eval_layout = new QGridLayout;

    // ajout en haut à gauche des onglets dans le layout de la fenêtre de configuration
    eval_layout->addWidget(onglets_eval, 0, 0);

    connect(onglets_eval,SIGNAL(currentChanged(int)),this,SLOT(compute(int)));

    // layout donné à la fenêtre de configuration
    evaluation_window->setLayout(eval_layout);

    ////////////settings pour evluation_window////////////////////

    scale_spinA->setValue(settings.value("Evaluation/spinA", 100).toInt());
    scale_spinB->setValue(settings.value("Evaluation/spinB", 100).toInt());

    Acolor_combobox->setCurrentIndex(settings.value("Evaluation/comboA", 0).toInt());
    Bcolor_combobox->setCurrentIndex(settings.value("Evaluation/comboB", 0).toInt());



    /////////// initialisation fenêtre about /////////////////////

    // création de la fenêtre about
    about_window = new QDialog(this);

    // titre pour la fenêtre de configuration
    about_window->setWindowTitle(tr("About Ofeli"));
    about_window->move(settings.value("aboutWindow/position", QPoint(200, 200)).toPoint());
    if( operating_system == 1 )
    {
        about_window->resize(629,328);
    }
    if( operating_system == 3 )
    {
        about_window->resize(639,338);
    }
    if( operating_system == 2 )
    {
        about_window->resize(742,366);
    }

    // widget permettant d'afficher l'image montrant les modifications des réglages
    QLabel* imageLabelAbout1 = new QLabel(this);
    imageLabelAbout1->setAlignment(Qt::AlignJustify | Qt::AlignVCenter);
    imageLabelAbout1->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::LinksAccessibleByMouse | Qt::LinksAccessibleByKeyboard);
    imageLabelAbout1->setOpenExternalLinks(true);
    imageLabelAbout1->setWordWrap(true);

    imageLabelAbout1->setText(tr("<p>Ofeli, as an acronym for <b>O</b>pen, <b>F</b>ast and <b>E</b>fficient <b>L</b>evel set "
                                 "<b>I</b>mplementation, demonstrates how to operate an image "
                                 "segmentation algorithm of Y. Shi and W. C. Karl <b>[1]</b>, using a "
                                 "discrete approach for the approximation of level-set-based "
                                 "curve evolution (implicit active contours).</p>"
                                 "<p>This is a novel (2005) and fast algorithm without the need "
                                 "of solving partial differential equations (PDE) while preserving the "
                                 "advantages of level set methods, such as the automatic handling of "
                                 "topological changes. Considerable speedups (×100) have been "
                                 "demonstrated as compared to PDE-based narrow band level-set implementations.</p>"
                                 "<hr>"
                                 "<p><b>[1]</b> Y. Shi, W. C. Karl - <a href='https://docs.google.com/viewer?a=v&pid=explorer&chrome=true&srcid=0Bzx5IoqehNE_MGIwYmUwYzctYTRkMC00ODMwLWI3YmUtNTFjYThlMTBkOTIy&hl=en&authkey=CPT1xeYN'>A real-time algorithm for the approximation of level-set based curve evolution</a> - <i>IEEE Trans. Image Processing</i>, vol. 17, no. 5, May 2008</p>"));


    QLabel* imageLabelAbout2 = new QLabel(this);
    imageLabelAbout2->setAlignment(Qt::AlignJustify | Qt::AlignVCenter);
    imageLabelAbout2->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::LinksAccessibleByMouse | Qt::LinksAccessibleByKeyboard);
    imageLabelAbout2->setOpenExternalLinks(true);
    imageLabelAbout2->setWordWrap(true);

    imageLabelAbout2->setText(tr("<p>This application has been developed by Fabien Bessy, under the supervision of Julien Olivier "
                                 "and Romuald Boné, during an internship in the <i>pattern recognition and image analysis research</i> "
                                 "<i>team (RFAI-LI)</i> of the <i>François Rabelais University's computer science laboratory</i>, at Tours, "
                                 "as part of the MSc in medical imaging of the same university, in 2010.</p>"
                                 "<p>If you have any questions, comments, or suggestions, please contact me via my email "
                                 "address : <a href='mailto:fabien.bessy@gmail.com'>fabien.bessy@gmail.com</a>.</p>"));



    // variable de la langue du système
    QString locale = QLocale::system().name().section('_', 0, 0);
    int language = settings.value("language",0).toInt();

    QString txt_file;
    if( language == 2 || ( language == 0 && locale == "fr" ) )
    {
        txt_file=QString(":licence.txt");
    }
    else
    {
        txt_file=QString(":license.txt");
    }
    QFile file(txt_file);
    QTextEdit* license_textedit = new QTextEdit(this);


    if( file.open(QIODevice::ReadOnly) )
    {
        QTextStream ts(&file);
        ts.setCodec("ISO 8859-1");
        license_textedit->setText( ts.readAll() );
    }

    license_textedit->setReadOnly(true);

    QLabel* imageLabelAbout3 = new QLabel(this);
    imageLabelAbout3->setAlignment(Qt::AlignJustify | Qt::AlignVCenter);
    imageLabelAbout3->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::LinksAccessibleByMouse | Qt::LinksAccessibleByKeyboard);
    imageLabelAbout3->setOpenExternalLinks(true);
    imageLabelAbout3->setWordWrap(true);

    imageLabelAbout3->setText(tr("<p>Thanks to :</p>"
                              "<p>- J. Olivier, R. Boné, J-M. Girault, F. Amed, A. Lissy, "
                              "C. Rouzière, L. Suta.</p>"
                              "<p>- <i>pattern recognition and image analysis research team</i>, <i>computer science laboratory</i>, "
                              "<i>François Rabelais University</i>.</p>"
                              "<p>- students and professors of the MSc in medical imaging.</p>"));


    QVBoxLayout* right_layout1 = new QVBoxLayout;
    right_layout1->addWidget(imageLabelAbout1);


    QVBoxLayout* right_layout2 = new QVBoxLayout;
    right_layout2->addWidget(imageLabelAbout2);

    QVBoxLayout* right_layout3 = new QVBoxLayout;
    right_layout3->addWidget(imageLabelAbout3);

    onglets1 = new QTabWidget(about_window);





    // création de widgets correspondant à chaque page ou à chaque onglet du QTabWidget onglets
    QWidget* page11 = new QWidget;
    QWidget* page22 = new QWidget;
    QWidget* page33 = new QWidget;

    // widget contenant des layouts
    page11->setLayout(right_layout1);
    page22->setLayout(right_layout2);
    page33->setLayout(right_layout3);

    // ajout des 4 widgets correspondant aux 4 onglets (aux 4 pages) dans le QTabWidget avec un titre pour chaque onglet
    onglets1->addTab(page11, tr("Description"));
    onglets1->addTab(page22, tr("Author"));
    onglets1->addTab(page33, tr("Acknowledgments"));



    QLabel* imag1 = new QLabel(this);
    imag1->setText("<b>Ofeli</b>");
    QFont font;
    font.setPointSize(24);
    font.setBold(true);
    imag1->setFont(font);
    imag1->setAlignment(Qt::AlignCenter);
    imag1->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::LinksAccessibleByMouse | Qt::LinksAccessibleByKeyboard);

    QLabel* imag2 = new QLabel(this);
    QFont font2;
    font2.setPointSize(12);
    imag2->setFont(font2);
    imag2->setText("Version 1.0.7");
    imag2->setAlignment(Qt::AlignCenter);
    imag2->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::LinksAccessibleByMouse | Qt::LinksAccessibleByKeyboard);

    QLabel* imag3 = new QLabel(this);
    QFont font3;
    font3.setPointSize(12);
    imag3->setFont(font3);
    imag3->setText("2010-2012");
    imag3->setAlignment(Qt::AlignCenter);
    imag3->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::LinksAccessibleByMouse | Qt::LinksAccessibleByKeyboard);

    QLabel* icon_label = new QLabel(this);
    QImage icon(":Ofeli.png");
    QPixmap icon_pixmap=QPixmap::fromImage(icon);
    icon_label->setPixmap(icon_pixmap);
    icon_label->setAlignment(Qt::AlignCenter);

    QPushButton* webpage = new QPushButton(tr("Web page"));
    webpage->setAutoDefault(false);
    QPushButton* license = new QPushButton(tr("License"));
    license->setAutoDefault(false);

    QVBoxLayout* layout1 = new QVBoxLayout;
    layout1->addWidget(icon_label);
    layout1->addWidget(imag1);
    layout1->addWidget(imag2);
    layout1->addWidget(imag3);
    layout1->addSpacing(10);
    layout1->addWidget(webpage);
    layout1->addWidget(license);
    layout1->addStretch(1);

    license_window = new QDialog(this);
    license_window->setWindowTitle(tr("License"));

    QHBoxLayout* layout_license = new QHBoxLayout;
    layout_license->addWidget(license_textedit);
    license_window->setLayout(layout_license);
    license_window->setMinimumWidth(562);
    license_window->setMaximumWidth(562);
    license_window->setMinimumHeight(351);
    license_window->resize(settings.value("licenseWindow/size", QSize(562,351)).toSize());
    license_window->move(settings.value("licenseWindow/position", QPoint(200, 200)).toPoint());

    connect( webpage, SIGNAL(clicked()), this, SLOT(openWebpage()) );
    connect( license, SIGNAL(clicked()), this, SLOT(openLicense()) );

    QHBoxLayout* layout0 = new QHBoxLayout;
    layout0->addLayout(layout1);
    layout0->addWidget(onglets1);

    about_window->setLayout(layout0);

    nameFilters << "*.bmp"
                //<< "*.dcm"
                << "*.gif"
                << "*.jpg" << "*.jpeg" << "*.mng"
                << "*.pbm" << "*.png" << "*.pgm"
                << "*.ppm" << "*.svg" << "*.svgz"
                << "*.tiff" << "*.tif" << "*.xbm" << "*.xpm";

    nameFilters.removeDuplicates();

    language_window = new QDialog(this);
    language_window->setWindowTitle(tr("Language"));

    listWidget = new QListWidget();

    listWidget->addItem(tr("System (")+locale+")");
    listWidget->addItem(tr("English"));
    listWidget->addItem(tr("French"));
    listWidget->setCurrentRow(settings.value("language",0).toInt());

    language_button = new QDialogButtonBox(language_window);
    language_button->addButton(QDialogButtonBox::Ok);
    language_button->setCenterButtons(true);
    connect( language_button, SIGNAL(accepted()), language_window, SLOT(accept()) );

    QLabel* restart_label = new QLabel(this);
    restart_label->setAlignment(Qt::AlignJustify);
    restart_label->setWordWrap(true);
    restart_label->setText(tr("The change will take effect after restarting the application."));

    QVBoxLayout* language_layout = new QVBoxLayout;
    language_layout->addWidget(listWidget);
    language_layout->addWidget(restart_label);
    language_layout->addWidget(language_button);


    language_window->setLayout(language_layout);

    statusBar()->clearMessage();
}

void ImageViewer::openWebpage()
{
    bool result = QDesktopServices::openUrl(QUrl("http://www.code.google.com/p/ofeli/", QUrl::TolerantMode));
}

void ImageViewer::openLicense()
{
    license_window->show();
}

void ImageViewer::openFileName()
{
    fileName = QFileDialog::getOpenFileName(this,
                                            tr("Open File"),
                                            last_directory_used,
                                            tr("Image Files (%1)").arg(nameFilters.join(" ")));


    open();
}

// Fonction appelée lorsqu'on ouvre une image
void ImageViewer::open()
{
    if( !fileName.isEmpty() )
    {
        QFileInfo fi(fileName);

        // if you want a partial support of DICOM image, without support of JPEG-2000 encapsulated image
        // install DCMTK library to the project (there is the folder "external_libraries")
        // and uncomment below

        /*if( fi.completeSuffix() == "dcm" || fi.completeSuffix() == "DCM" )
        {
            DicomImage* dcm_img = new DicomImage( fileName.toStdString().c_str() );

            if( dcm_img != NULL)
            {
                if( dcm_img->getStatus() == EIS_Normal )
                {
                    int width = dcm_img->getWidth();
                    int height = dcm_img->getHeight();
                    unsigned char* pixelData = (unsigned char*)( dcm_img->getOutputData(8) );

                    if( pixelData != NULL )
                    {
                        img = QImage(width,height,QImage::Format_Indexed8);

                        int size = width*height;
                        int x, y;
                        for( int offset = 0; offset < size; offset++ )
                        {
                            y = offset/width;
                            x = offset-y*width;

                            *( img.scanLine(y)+x ) = pixelData[offset];
                        }
                    }
                }
                else
                {
                    std::cerr << "Error: cannot load DICOM image (" << DicomImage::getString(dcm_img->getStatus()) << ")" << std::endl;
                }
            }
            delete dcm_img;
        }*/
        //else
        //{
            img = QImage(fileName);
            if( img.isNull() )
            {
                QMessageBox::information(this, tr("Opening error - Ofeli"),
                                         tr("Cannot load %1.").arg(fileName));
                return;
            }
        //}

        image_format = img.format();

        if( image_format == 3 ) // si l'image est en niveau de gris
        {
            QVector<QRgb> table(256);
            for( int I = 0; I < 256; I++ )
            {
                table[I] = qRgb(I,I,I);
            }
            img.setColorTable(table);
        }

        imageLabel->set_qimage0(img);
        QApplication::processEvents();
        statusBar()->showMessage(tr("Push the left mouse button or click on the right mouse button in the window."));

        imageLabel_settings->set_qimage0(img);

        iteration1 = -1;

        QString name = fi.fileName();
        last_directory_used = fi.absolutePath();
        setCurrentFile(fileName);

        img_width = img.width();
        img_height = img.height();
        img_size = img_width*img_height;

        positionX = img_width/2;
        positionY = img_height/2;

        phi_text->setText("ϕ(x,y,t) ∈ { -3, -1, 1, 3 }");
        text->setToolTip(tr("must be strictly less than iteration_max = 5 × max(img_width,img_height) = ")+QString::number(5*std::max(img_width,img_height)));

        if( ac != NULL )
        {
            delete ac;
            ac = NULL;
        }

        image_disp = 1;
        hasResultDisp = false;

        QString string_lists_text;
        if( image_format == 3 )
        {
            string_lists_text = QString::number(img_width)+"×"+QString::number(img_height)+"×1";
        }
        else
        {
            string_lists_text = QString::number(img_width)+"×"+QString::number(img_height)+"×3";
        }

        if( operating_system == 2 )
        {
            setWindowTitle(name+" - "+string_lists_text);
        }
        if( operating_system == 1 || operating_system == 3 )
        {
            setWindowTitle(name+" - "+string_lists_text+" - Ofeli");
        }


        // Paramètres d'entrée du contour actif

        // Création de img1
        if( img1 != NULL )
        {
            delete[] img1;
            img1 = NULL;
        }

        int x, y; // current position of the pixel

        if( image_format == 3 ) // si l'image est en niveau de gris
        {
            isRgb1 = false;

            img1 = new unsigned char[img_size];

            for( int offset = 0; offset < img_size; offset++ )
            {
                // offset = x+y*img_width <==>
                y = offset/img_width;
                x = offset-y*img_width;

                img1[offset] = *( img.scanLine(y)+x );
            }
        }
        else // si l'image est en couleur
        {
            if( img.isGrayscale() ) // si les 3 composantes sont identiques
            {
                isRgb1 = false;

                img1 = new unsigned char[img_size];

                QRgb pix;

                for( int offset = 0; offset < img_size; offset++ )
                {
                    // offset = x+y*img_width <==>
                    y = offset/img_width;
                    x = offset-y*img_width;

                    pix = img.pixel(x,y);

                    img1[offset] = (unsigned char)(qRed(pix));
                }
            }
            else
            {
                isRgb1 = true;

                img1 = new unsigned char[3*img_size];

                QRgb pix;

                for( int offset = 0; offset < img_size; offset++ )
                {
                    // offset = x+y*img_width <==>
                    y = offset/img_width;
                    x = offset-y*img_width;

                    pix = img.pixel(x,y);

                    img1[3*offset] = (unsigned char)(qRed(pix));
                    img1[3*offset+1] = (unsigned char)(qGreen(pix));
                    img1[3*offset+2] = (unsigned char)(qBlue(pix));
                }
            }
        }

        // création de l'image qui va être affichée et modifiée
        if( image_uchar != NULL )
        {
            delete[] image_uchar;
            image_uchar = NULL;
        }
        if( image_uchar == NULL )
        {
            image_uchar = new unsigned char[3*img_size];
        }
        image = QImage(image_uchar, img_width, img_height, 3*img_width, QImage::Format_RGB888);

        if( isRgb1 )
        {
            std::memcpy(image_uchar,img1,3*img_size);
        }
        else
        {
            unsigned char I;

            for( int i = 0; i < img_size; i++ )
            {
                I = img1[i];

                image_uchar[3*i] = I;
                image_uchar[3*i+1] = I;
                image_uchar[3*i+2] = I;
            }
        }

        if( shape != NULL )
        {
            delete[] shape;
            shape = NULL;
        }
        if( shape == NULL )
        {
            shape = new char[img_size];
        }

        if( shape_points != NULL )
        {
            delete[] shape_points;
            shape_points = NULL;
        }
        if( shape_points == NULL )
        {
            shape_points = new int[2*img_size+1];
            shape_points[0] = list_end;
        }

        if( Lout_shape1 != NULL )
        {
            delete[] Lout_shape1;
            Lout_shape1 = NULL;
        }
        if( Lout_shape1 == NULL )
        {
            Lout_shape1 = new int[img_size+1];
            Lout_shape1[0] = list_end;
        }

        if( Lin_shape1 != NULL )
        {
            delete[] Lin_shape1;
            Lin_shape1 = NULL;
        }
        if( Lin_shape1 == NULL )
        {
            Lin_shape1 = new int[img_size+1];
            Lin_shape1[0] = list_end;
        }

        if( Lout_2 != NULL )
        {
            delete[] Lout_2;
            Lout_2 = NULL;
        }
        if( Lout_2 == NULL )
        {
            Lout_2 = new int[img_size+1];
            Lout_2[0] = list_end;
        }

        if( Lin_2 != NULL )
        {
            delete[] Lin_2;
            Lin_2 = NULL;
        }
        if( Lin_2 == NULL )
        {
            Lin_2 = new int[img_size+1];
            Lin_2[0] = list_end;
        }

        if( phi_init2 != NULL )
        {
            delete[] phi_init2;
            phi_init2 = NULL;
        }
        if( phi_init2 == NULL )
        {
            phi_init2 = new char[img_size];
        }

        if( phi_init1_clean != NULL )
        {
            delete[] phi_init1_clean;
            phi_init1_clean = NULL;
        }
        if( phi_init1_clean == NULL )
        {
            phi_init1_clean = new char[img_size];
        }

        if( phi_init2_clean != NULL )
        {
            delete[] phi_init2_clean;
            phi_init2_clean = NULL;
        }
        if( phi_init2_clean == NULL )
        {
            phi_init2_clean = new char[img_size];
        }

        if( phi_init1 != NULL )
        {
            delete[] phi_init1;
            phi_init1 = NULL;
        }
        if( phi_init1 == NULL )
        {
            phi_init1 = new char[img_size];
        }

        if( image_result_uchar != NULL )
        {
            delete[] image_result_uchar;
            image_result_uchar = NULL;
        }
        if( image_result_uchar == NULL )
        {
            image_result_uchar = new unsigned char[3*img_size];
        }
        image_result = QImage(image_result_uchar, img_width, img_height, 3*img_width,QImage::Format_RGB888);

        if( image_filter_uchar != NULL )
        {
            delete[] image_filter_uchar;
            image_filter_uchar = NULL;
        }
        if( image_filter_uchar == NULL )
        {
            image_filter_uchar = new unsigned char[3*img_size];
        }
        image_filter = QImage(image_filter_uchar, img_width, img_height, 3*img_width, QImage::Format_RGB888);

        if( image_phi_uchar != NULL )
        {
            delete[] image_phi_uchar;
            image_phi_uchar = NULL;
        }
        if( image_phi_uchar == NULL )
        {
            image_phi_uchar = new unsigned char[3*img_size];
        }
        image_phi = QImage(image_phi_uchar, img_width, img_height, 3*img_width, QImage::Format_RGB888);

        if( image_shape_uchar != NULL )
        {
            delete[] image_shape_uchar;
            image_shape_uchar = NULL;
        }
        if( image_shape_uchar == NULL )
        {
            image_shape_uchar = new unsigned char[3*img_size];
        }
        image_shape = QImage(image_shape_uchar, img_width, img_height, 3*img_width, QImage::Format_RGB888);

        if( isRgb1 )
        {
            image_save_preprocess = QImage(img_width, img_height, QImage::Format_RGB888);
        }
        else
        {
            image_save_preprocess = QImage(img_width,img_height, QImage::Format_Indexed8);
        }

        QImage img_phi0 = QImage(100,100, QImage::Format_Indexed8);

        for( int offset = 0; offset < 10000; offset++ )
        {
            // offset = x+y*img_width <==>
            y = offset/100;
            x = offset-y*100;

            if( x < 95 && x > 5 && y < 95 && y > 5 )
            {
                *( img_phi0.scanLine(y)+x ) = 255;
            }
            else
            {
                *( img_phi0.scanLine(y)+x ) = 0;
            }
        }

        QSettings settings("Bessy","Ofeli");

        if( img_phi.isNull() )
        {
            img_phi = settings.value("Settings/Initialization/img_phi",img_phi0).value<QImage>();
        }

        if( img_phi.isNull() )
        {
            img_phi = img_phi0.scaled(img_width, img_height, Qt::IgnoreAspectRatio, Qt::FastTransformation);
        }
        else
        {
            img_phi = img_phi.scaled(img_width, img_height, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        }

        imgPhi2phiInit();

        int n_out = 0;
        int n_in = 0;
        for( int offset = 0; offset < img_size; offset++ )
        {
            if( phi_init1_clean[offset] == 1 )
            {
                Lout_2[n_out++] = offset;
            }
            if( phi_init1_clean[offset] == -1 )
            {
                Lin_2[n_in++] = offset;
            }
        }
        Lout_2[n_out] = list_end;
        Lin_2[n_in] = list_end;


        tab_visu(onglets->currentIndex());


        if( isRgb1 && model == 1 )
        {
            Cout_text->setText("Cout = (<font color=red>R<font color=black>,<font color=green>G<font color=black>,<font color=blue>B<font color=black>)");
            Cin_text->setText("Cin = (<font color=red>R<font color=black>,<font color=green>G<font color=black>,<font color=blue>B<font color=black>)");

            Cout_text->setToolTip(tr("outside average of the image in the (R,G,B) color space"));
            Cin_text->setToolTip(tr("inside average of the image in (R,G,B) color space"));
        }
        if( !isRgb1 && model == 1 )
        {
            Cout_text->setText("Cout =");
            Cin_text->setText("Cin =");

            Cout_text->setToolTip(tr("outside average of the image"));
            Cin_text->setToolTip(tr("inside average of the image"));
        }

        if( isRgb1 )
        {
            yuv_groupbox->setHidden(false);
            pixel_text->setText(tr("img(x,y) = (<font color=red>R<font color=black>,<font color=green>G<font color=black>,<font color=blue>B<font color=black>)"));
            pixel_text->setToolTip(tr("pixel value at the position (x,y) in the (R,G,B) color space"));
        }
        else
        {
            yuv_groupbox->setHidden(true);
            pixel_text->setText(tr("img(x,y) = I"));
            pixel_text->setToolTip(tr("pixel value at the position (x,y)"));
        }

        lists_text->setText("Lin(t) & Lout(t)");

        if( filters2 != NULL )
        {
            delete filters2;
            filters2 = NULL;
        }

        if( filters2 == NULL )
        {
            if( isRgb1 )
            {
                filters2 = new ofeli::Filters(img1, img_width, img_height, 3);
            }
            else
            {
                filters2 = new ofeli::Filters(img1, img_width, img_height, 1);
            }
        }

        disconnect(scale_spin,SIGNAL(valueChanged(int)),this,SLOT(do_scale(int)));
        disconnect(scale_spin0,SIGNAL(valueChanged(int)),this,SLOT(do_scale0(int)));

        scale_spin->setMaximum(1000000/img_height);
        scale_spin->setSingleStep(80000/(7*img_height));
        scale_spin0->setMaximum(1000000/img_height);
        scale_spin0->setSingleStep(80000/(7*img_height));

        scale_slider->setMaximum(160000/img_height);
        scale_slider->setTickInterval(160000/(7*img_height));
        scale_slider0->setMaximum(160000/img_height);
        scale_slider0->setTickInterval(160000/(7*img_height));

        scale_spin0->setValue(int(100.0*double(imageLabel->getPixWidth())/double(img_width)));

        connect(scale_spin,SIGNAL(valueChanged(int)),this,SLOT(do_scale(int)));
        connect(scale_spin0,SIGNAL(valueChanged(int)),this,SLOT(do_scale0(int)));

        if( img1 != NULL )
        {
            hasAlgoBreaking = false;
            hasStepByStep = false;
            hasClickStopping = false;
            printAct->setEnabled(true);
            zoomInAct->setEnabled(true);
            zoomOutAct->setEnabled(true);
            normalSizeAct->setEnabled(true);
            startAct->setEnabled(true);
            open_phi_button->setEnabled(true);
            save_phi_button->setEnabled(true);
            saveAct->setEnabled(true);
        }
    }
}

void ImageViewer::print()
//! [5] //! [6]
{
    Q_ASSERT(imageLabel->pixmap());
#ifndef QT_NO_PRINTER
//! [6] //! [7]
    QPrintDialog dialog(&printer, this);
//! [7] //! [8]
    if (dialog.exec()) {
        QPainter painter(&printer);
        QRect rect = painter.viewport();
        QSize size = imageLabel->pixmap()->size();
        size.scale(rect.size(), Qt::KeepAspectRatio);
        painter.setViewport(rect.x(), rect.y(), size.width(), size.height());
        painter.setWindow(imageLabel->pixmap()->rect());
        painter.drawPixmap(0, 0, *imageLabel->pixmap());
    }
#endif
}

void ImageViewer::zoomIn()
{
    //scaleImage(1.25);
    scale_spin0->setValue( int(100.0f*imageLabel->get_zoomFactor()*1.25f) );
}

void ImageViewer::zoomOut()
{
    //scaleImage(0.8);
    scale_spin0->setValue( int(100.0f*imageLabel->get_zoomFactor()*0.8f) );
}

void ImageViewer::normalSize()
{
    //imageLabel->adjustSize();
    //scaleFactor = 1.0;
    scale_spin0->setValue(100);
}

// Fonction appelée pour effectuer la segmentation
void ImageViewer::start()
{
    startAct->setEnabled(false);
    openAct->setEnabled(false);
    stopAct->setEnabled(true);

    if( img1 != NULL )
    {
        if( filters1 != NULL )
        {
            delete filters1;
            filters1 = NULL;
        }

        if( filters1 == NULL )
        {
            if( isRgb1 )
            {
                filters1 = new ofeli::Filters(img1, img_width, img_height, 3);
            }
            else
            {
                filters1 = new ofeli::Filters(img1, img_width, img_height, 1);
            }
        }

        if( hasPreprocess )
        {
            if( hasGaussianNoise )
            {
                filters1->gaussian_white_noise(std_noise);
            }
            if( hasSaltNoise )
            {
                filters1->salt_pepper_noise(proba_noise);
            }
            if( hasSpeckleNoise )
            {
                filters1->speckle(std_speckle_noise);
            }

            if( hasMeanFilt )
            {
                filters1->mean_filtering(kernel_mean_length1);
            }
            if( hasGaussianFilt )
            {
                filters1->gaussian_filtering(kernel_gaussian_length1, sigma);
            }

            if( hasMedianFilt )
            {
                if( hasO1algo1 )
                {
                    filters1->median_filtering_o1(kernel_median_length1);
                }
                else
                {
                    filters1->median_filtering_oNlogN(kernel_median_length1);
                }
            }
            if( hasAnisoDiff )
            {
                filters1->anisotropic_diffusion(max_itera1, lambda1, kappa1, aniso_option1);
            }
            if( hasOpenFilt )
            {
                if( hasO1morpho1 )
                {
                    filters1->opening_o1(kernel_open_length1);
                }
                else
                {
                    filters1->opening(kernel_open_length1);
                }
            }
            if( hasCloseFilt )
            {
                if( hasO1morpho1 )
                {
                    filters1->closing_o1(kernel_close_length1);
                }
                else
                {
                    filters1->closing(kernel_close_length1);
                }
            }
            if( hasTophatFilt )
            {
                if( isWhiteTophat )
                {
                    if( hasO1morpho1 )
                    {
                        filters1->white_top_hat_o1(kernel_tophat_length1);
                    }
                    else
                    {
                        filters1->white_top_hat(kernel_tophat_length1);
                    }
                }
                else
                {
                    if( hasO1morpho1 )
                    {
                        filters1->black_top_hat_o1(kernel_tophat_length1);
                    }
                    else
                    {
                        filters1->black_top_hat(kernel_tophat_length1);
                    }
                }
            }
        }

        const unsigned char* img1_filtered = NULL;

        if( model == 1 )
        {
            img1_filtered = filters1->get_filtered();
        }
        if( model == 2 )
        {
            if( isRgb1 )
            {
                filters1->morphological_gradient_yuv(kernel_gradient_length1,alpha1,beta1,gamma1);
                img1_filtered = filters1->get_gradient();
            }
            else
            {
                filters1->morphological_gradient(kernel_gradient_length1);
                img1_filtered = filters1->get_filtered();
            }
        }

        /*if( show_original )
        {
            img1_filtered = img1;
        }*/


        int offset;

        unsigned char I;

        // if not a new image is loaded
        if( ac != NULL )
        {
            // efface les contours de la QImage
            if( !isRgb1 || model == 2 )
            {
                if( model == 2 && hasHistoNormaliz )
                {
                    int min, max;

                    if( outside_combo != 9 )
                    {
                        for( ofeli::list<int>::const_iterator it = Lout1->begin(); !it.end(); ++it )
                        {
                            offset = *it;

                            I = (unsigned char)(255.0*double(img1_filtered[offset]-min)/double(max-min));

                            image_result_uchar[3*offset] = I;
                            image_result_uchar[3*offset+1] = I;
                            image_result_uchar[3*offset+2] = I;
                        }
                    }

                    if( inside_combo != 9 )
                    {
                        for( ofeli::list<int>::const_iterator it = Lin1->begin(); !it.end(); ++it )
                        {
                            offset = *it;

                            I = (unsigned char)(255.0*double(img1_filtered[offset]-min)/double(max-min));

                            image_result_uchar[3*offset] = I;
                            image_result_uchar[3*offset+1] = I;
                            image_result_uchar[3*offset+2] = I;
                        }
                    }
                }

                else
                {
                    if( outside_combo != 9 )
                    {
                        for( ofeli::list<int>::const_iterator it = Lout1->begin(); !it.end(); ++it )
                        {
                            offset = *it;

                            I = img1_filtered[offset];

                            image_result_uchar[3*offset] = I;
                            image_result_uchar[3*offset+1] = I;
                            image_result_uchar[3*offset+2] = I;
                        }
                    }

                    if( inside_combo != 9 )
                    {
                        for( ofeli::list<int>::const_iterator it = Lin1->begin(); !it.end(); ++it )
                        {
                            offset = *it;

                            I = img1_filtered[offset];

                            image_result_uchar[3*offset] = I;
                            image_result_uchar[3*offset+1] = I;
                            image_result_uchar[3*offset+2] = I;
                        }
                    }
                }
            }
            else
            {
                if( outside_combo != 9 )
                {
                    for( ofeli::list<int>::const_iterator it = Lout1->begin(); !it.end(); ++it )
                    {
                        offset = *it*3;

                        image_result_uchar[offset] = img1_filtered[offset];
                        image_result_uchar[offset+1] = img1_filtered[offset+1];
                        image_result_uchar[offset+2] = img1_filtered[offset+2];
                    }
                }

                if( inside_combo != 9 )
                {
                    for( ofeli::list<int>::const_iterator it = Lin1->begin(); !it.end(); ++it )
                    {
                        offset = *it*3;

                        image_result_uchar[offset] = img1_filtered[offset];
                        image_result_uchar[offset+1] = img1_filtered[offset+1];
                        image_result_uchar[offset+2] = img1_filtered[offset+2];
                    }
                }
            }
        }

        // Création d'un objet contour actif

        // objets crees en dynamique et appel du 2eme constructeur
        if( ac != NULL )
        {
            delete ac;
            ac = NULL;
        }

        if( model == 1 )
        {
            if( isRgb1 )
            {
                ac = new ofeli::ACwithoutEdgesYUV(img1_filtered, img_width, img_height, phi_init1_clean, hasSmoothingCycle1, kernel_curve1, std_curve1, Na1, Ns1, lambda_out1, lambda_in1, alpha1, beta1, gamma1);
            }
            else
            {
                ac = new ofeli::ACwithoutEdges(img1_filtered, img_width, img_height, phi_init1_clean, hasSmoothingCycle1, kernel_curve1, std_curve1, Na1, Ns1, lambda_out1, lambda_in1);
            }
        }

        if( model == 2 )
        {
            ac = new ofeli::GeodesicAC(img1_filtered, img_width, img_height, phi_init1_clean, hasSmoothingCycle1, kernel_curve1, std_curve1, Na1, Ns1);
        }

        show_phi_list_value();

        // Conditions de la boucle do-while

        iteration1 = ac->get_iteration();

        if( hasAlgoBreaking )
        {
            hasAlgoBreaking = false;
            startAct->setEnabled(true);
            openAct->setEnabled(true);
            stopAct->setEnabled(false);
            return;
        }

        if( outside_combo == 8 )
        {
            Rout1 = Rout_selected1;
            Gout1 = Gout_selected1;
            Bout1 = Bout_selected1;
        }
        else
        {
            get_color(outside_combo,Rout1,Gout1,Bout1);
        }

        if( inside_combo == 8 )
        {
            Rin1 = Rin_selected1;
            Gin1 = Gin_selected1;
            Bin1 = Bin_selected1;
        }
        else
        {
            get_color(inside_combo,Rin1,Gin1,Bin1);
        }

        Lout1 = &ac->get_Lout();
        Lin1 = &ac->get_Lin();

        time_text->setText(tr("time ="));

        if( model == 1 )
        {
            stackedWidget->setCurrentIndex(0);
        }

        if( model == 2 )
        {
            stackedWidget->setCurrentIndex(1);
        }

        //////////////////////////////////////////////////////////////////////
        // affiche une fois le contour actif avec l'image filtrée en dessous
        //////////////////////////////////////////////////////////////////////

        unsigned char max, min;

        if( !isRgb1 || model == 2 )
        {
            if( model == 2 && hasHistoNormaliz )
            {
                max = 0;
                min = 255;

                for( offset = 0; offset < img_size; offset++ )
                {
                    if( img1_filtered[offset] > max )
                    {
                        max = img1_filtered[offset];
                    }
                    if( img1_filtered[offset] < min )
                    {
                        min = img1_filtered[offset];
                    }
                }

                for( offset = 0; offset < img_size; offset++ )
                {
                    I = (unsigned char)(255.0*double(img1_filtered[offset]-min)/double(max-min));

                    image_result_uchar[3*offset] = I;
                    image_result_uchar[3*offset+1] = I;
                    image_result_uchar[3*offset+2] = I;
                }
            }

            else
            {
                for( offset = 0; offset < img_size; offset++ )
                {
                    I = img1_filtered[offset];

                    image_result_uchar[3*offset] = I;
                    image_result_uchar[3*offset+1] = I;
                    image_result_uchar[3*offset+2] = I;
                }
            }
        }
        else
        {
            std::memcpy(image_result_uchar,img1_filtered,3*img_size);
        }

        if( hasGaussianNoise || hasSaltNoise || hasSpeckleNoise || hasMeanFilt || hasGaussianFilt || hasMedianFilt || hasAnisoDiff || hasOpenFilt || hasCloseFilt || hasTophatFilt || model == 2 )
        {
            if( isRgb1 )
            {
                image_save_preprocess = image_result.copy(0,0,img_width,img_height);
            }
            else
            {
                QVector<QRgb> table(256);
                for( int I = 0; I < 256; I++ )
                {
                    table[I] = qRgb(I,I,I);
                }
                image_save_preprocess.setColorTable(table);

                int x, y; // position

                for( int offset = 0; offset < img_size; offset++ )
                {
                    // offset = x+y*img_width <==>
                    y = offset/img_width;
                    x = offset-y*img_width;

                    *( image_save_preprocess.scanLine(y)+x ) = img1_filtered[offset];
                }
            }
        }

        // marque les contours
        if( outside_combo != 9 )
        {
            for( ofeli::list<int>::const_iterator it = Lout1->begin(); !it.end(); ++it )
            {
                offset = *it*3;

                image_result_uchar[offset] = Rout1;
                image_result_uchar[offset+1] = Gout1;
                image_result_uchar[offset+2] = Bout1;
            }
        }

        if( inside_combo != 9 )
        {
            for( ofeli::list<int>::const_iterator it = Lin1->begin(); !it.end(); ++it )
            {
                offset = *it*3;

                image_result_uchar[offset] = Rin1;
                image_result_uchar[offset+1] = Gin1;
                image_result_uchar[offset+2] = Bin1;
            }
        }

        image_disp = 2;

        QApplication::processEvents();
        // Affichage
        imageLabel->set_qimage(image_result);

        if( hasStepByStep )
        {
            hasClickStopping = true;
        }
        infinite_loop();

        // efface les contours de la QImage
        if( !isRgb1 || model == 2 )
        {
            if( model == 2 && hasHistoNormaliz )
            {
                if( outside_combo != 9 )
                {
                    for( ofeli::list<int>::const_iterator it = Lout1->begin(); !it.end(); ++it )
                    {
                        offset = *it;

                        I = (unsigned char)(255.0*double(img1_filtered[offset]-min)/double(max-min));

                        image_result_uchar[3*offset] = I;
                        image_result_uchar[3*offset+1] = I;
                        image_result_uchar[3*offset+2] = I;
                    }
                }

                if( inside_combo != 9 )
                {
                    for( ofeli::list<int>::const_iterator it = Lin1->begin(); !it.end(); ++it )
                    {
                        offset = *it;

                        I = (unsigned char)(255.0*double(img1_filtered[offset]-min)/double(max-min));

                        image_result_uchar[3*offset] = I;
                        image_result_uchar[3*offset+1] = I;
                        image_result_uchar[3*offset+2] = I;
                    }
                }
            }

            else
            {
                if( outside_combo != 9 )
                {
                    for( ofeli::list<int>::const_iterator it = Lout1->begin(); !it.end(); ++it )
                    {
                        offset = *it;

                        I = img1_filtered[offset];

                        image_result_uchar[3*offset] = I;
                        image_result_uchar[3*offset+1] = I;
                        image_result_uchar[3*offset+2] = I;
                    }
                }

                if( inside_combo != 9 )
                {
                    for( ofeli::list<int>::const_iterator it = Lin1->begin(); !it.end(); ++it )
                    {
                        offset = *it;

                        I = img1_filtered[offset];

                        image_result_uchar[3*offset] = I;
                        image_result_uchar[3*offset+1] = I;
                        image_result_uchar[3*offset+2] = I;
                    }
                }
            }
        }
        else
        {
            if( outside_combo != 9 )
            {
                for( ofeli::list<int>::const_iterator it = Lout1->begin(); !it.end(); ++it )
                {
                    offset = *it*3;

                    image_result_uchar[offset] = img1_filtered[offset];
                    image_result_uchar[offset+1] = img1_filtered[offset+1];
                    image_result_uchar[offset+2] = img1_filtered[offset+2];
                }
            }

            if( inside_combo != 9 )
            {
                for( ofeli::list<int>::const_iterator it = Lin1->begin(); !it.end(); ++it )
                {
                    offset = *it*3;

                    image_result_uchar[offset] = img1_filtered[offset];
                    image_result_uchar[offset+1] = img1_filtered[offset+1];
                    image_result_uchar[offset+2] = img1_filtered[offset+2];
                }
            }
        }

        //////////////////////////////////////////////////////////////////////
        // fin de
        //////////////////////////////////////////////////////////////////////

        imageLabel->setMouseTracking(false);

        double elapsedTime;
        std::clock_t startTime, stopTime;

        statusBar()->showMessage(tr("Active contour evolving."));
        time_text->setText("<font color=green>"+tr("time = _._____ s"));

        if( hasDisplayEach )
        {
            // Pour calculer le temps d'exécution
            startTime = std::clock();

            //////////////////////////////////////////////////////////////////////
            // Boucle do-while, évolution de l'algorithme
            //////////////////////////////////////////////////////////////////////
            while( !ac->get_isStopped() )
            {
                if( hasAlgoBreaking )
                {
                    hasAlgoBreaking = false;
                    startAct->setEnabled(true);
                    openAct->setEnabled(true);
                    stopAct->setEnabled(false);
                    return;
                }

                if( hasStepByStep )
                {
                    hasClickStopping = true;
                }

                // efface les contours de la QImage
                if( !isRgb1 || model == 2 )
                {
                    if( model == 2 && hasHistoNormaliz )
                    {
                        if( outside_combo != 9 )
                        {
                            for( ofeli::list<int>::const_iterator it = Lout1->begin(); !it.end(); ++it )
                            {
                                offset = *it;

                                I = (unsigned char)(255.0*double(img1_filtered[offset]-min)/double(max-min));

                                image_result_uchar[3*offset] = I;
                                image_result_uchar[3*offset+1] = I;
                                image_result_uchar[3*offset+2] = I;
                            }
                        }

                        if( inside_combo != 9 )
                        {
                            for( ofeli::list<int>::const_iterator it = Lin1->begin(); !it.end(); ++it )
                            {
                                offset = *it;

                                I = (unsigned char)(255.0*double(img1_filtered[offset]-min)/double(max-min));

                                image_result_uchar[3*offset] = I;
                                image_result_uchar[3*offset+1] = I;
                                image_result_uchar[3*offset+2] = I;
                            }
                        }

                    }

                    else
                    {
                        if( outside_combo != 9 )
                        {
                            for( ofeli::list<int>::const_iterator it = Lout1->begin(); !it.end(); ++it )
                            {
                                offset = *it;

                                I = img1_filtered[offset];

                                image_result_uchar[3*offset] = I;
                                image_result_uchar[3*offset+1] = I;
                                image_result_uchar[3*offset+2] = I;
                            }
                        }

                        if( inside_combo != 9 )
                        {
                            for( ofeli::list<int>::const_iterator it = Lin1->begin(); !it.end(); ++it )
                            {
                                offset = *it;

                                I = img1_filtered[offset];

                                image_result_uchar[3*offset] = I;
                                image_result_uchar[3*offset+1] = I;
                                image_result_uchar[3*offset+2] = I;
                            }
                        }
                    }
                }

                else
                {
                    if( outside_combo != 9 )
                    {
                        for( ofeli::list<int>::const_iterator it = Lout1->begin(); !it.end(); ++it )
                        {
                            offset = *it*3;

                            image_result_uchar[offset] = img1_filtered[offset];
                            image_result_uchar[offset+1] = img1_filtered[offset+1];
                            image_result_uchar[offset+2] = img1_filtered[offset+2];
                        }
                    }

                    if( inside_combo != 9 )
                    {
                        for( ofeli::list<int>::const_iterator it = Lin1->begin(); !it.end(); ++it )
                        {
                            offset = *it*3;

                            image_result_uchar[offset] = img1_filtered[offset];
                            image_result_uchar[offset+1] = img1_filtered[offset+1];
                            image_result_uchar[offset+2] = img1_filtered[offset+2];
                        }
                    }
                }

                ++(*ac);
                Lout1 = &ac->get_Lout();
                Lin1 = &ac->get_Lin();

                // marque les contours dans la QImage
                if( outside_combo != 9 )
                {
                    for( ofeli::list<int>::const_iterator it = Lout1->begin(); !it.end(); ++it )
                    {
                        offset = *it*3;

                        image_result_uchar[offset] = Rout1;
                        image_result_uchar[offset+1] = Gout1;
                        image_result_uchar[offset+2] = Bout1;
                    }
                }

                if( inside_combo != 9 )
                {
                    for( ofeli::list<int>::const_iterator it = Lin1->begin(); !it.end(); ++it )
                    {
                        offset = *it*3;

                        image_result_uchar[offset] = Rin1;
                        image_result_uchar[offset+1] = Gin1;
                        image_result_uchar[offset+2] = Bin1;
                    }
                }

                // Pour afficher plusieurs fois dans une boucle
                QApplication::processEvents();
                // Affichage
                imageLabel->update();

                if( hasClickStopping )
                {
                    show_phi_list_value();
                }
                infinite_loop();
            }

            // Temps d'exécution de la boucle do-while
            stopTime = std::clock();
            elapsedTime = (stopTime - startTime) / double(CLOCKS_PER_SEC);
        }
        // on affiche rien du tout pour evaluer le temps de calcul
        // if( !hasDisplayEach )
        else
        {
            startTime = std::clock();

            ac->evolve();
            Lout1 = &ac->get_Lout();
            Lin1 = &ac->get_Lin();

            // Temps d'exécution
            stopTime = std::clock();
            elapsedTime = (stopTime - startTime) / double(CLOCKS_PER_SEC);

            // marque les contours dans la QImage
            if( outside_combo != 9 )
            {
                for( ofeli::list<int>::const_iterator it = Lout1->begin(); !it.end(); ++it )
                {
                    offset = *it*3;

                    image_result_uchar[offset] = Rout1;
                    image_result_uchar[offset+1] = Gout1;
                    image_result_uchar[offset+2] = Bout1;
                }
            }
            if( inside_combo != 9 )
            {
                for( ofeli::list<int>::const_iterator it = Lin1->begin(); !it.end(); ++it )
                {
                    offset = *it*3;

                    image_result_uchar[offset] = Rin1;
                    image_result_uchar[offset+1] = Gin1;
                    image_result_uchar[offset+2] = Bin1;
                }
            }

            // Pour afficher plusieurs fois dans une boucle
            QApplication::processEvents();
            // Affichage
            imageLabel->update();

            if( hasClickStopping )
            {
                show_phi_list_value();
            }
            infinite_loop();
        }

        time_text->setText("<font color=red>"+tr("time = ")+QString::number(elapsedTime)+" s");
        show_phi_list_value();
        statusBar()->clearMessage();

        imageLabel->setMouseTracking(true);

        hasResultDisp = true;

        openAct->setEnabled(true);

        statusBar()->showMessage(tr("Push the left mouse button or click on the right mouse button in the window."));
    }

    hasStepByStep = false;
    startAct->setEnabled(true);
    stopAct->setEnabled(false);

}

void ImageViewer::wait(double seconds)
{
  QApplication::processEvents();

  std::clock_t endwait;
  endwait = std::clock()+seconds *CLOCKS_PER_SEC ;
  while( std::clock() < endwait )
  {
  }
}

// Quand on clique dans le menu sur l'entrée About, la fenêtre about_window s'affiche
void ImageViewer::about()
{
    about_window->show();
}

// Creation des actions de la barre de menu
void ImageViewer::createActions()
{
    openAct = new QAction(tr("&Open..."), this);
    openAct->setShortcut(tr("Ctrl+O"));
    openAct->setStatusTip(tr("Open an image file (*.png, *.bmp, *.jpg, *.jpeg, *.tiff, *.tif, *.gif, *.pbm, *.pgm, *.ppm, *.svg, *.svgz, *.mng, *.xbm, *.xpm)."));
    connect(openAct, SIGNAL(triggered()), this, SLOT(openFileName()));

    for( int i = 0; i < MaxRecentFiles; ++i )
    {
        recentFileActs[i] = new QAction(this);
        recentFileActs[i]->setVisible(false);
        connect(recentFileActs[i], SIGNAL(triggered()),
                this, SLOT(openRecentFile()));
    }

    deleteAct = new QAction(tr("×   Delete list"), this);
    deleteAct->setStatusTip(tr("Clean the recent files list."));
    connect(deleteAct, SIGNAL(triggered()), this, SLOT(deleteList()));

    saveAct = new QAction(tr("S&ave..."), this);
    saveAct->setShortcut(tr("Ctrl+A"));
    saveAct->setEnabled(false);
    saveAct->setStatusTip(tr("Save the displayed and preprocessed images."));
    connect(saveAct, SIGNAL(triggered()), this, SLOT(saveImage()));

    printAct = new QAction(tr("&Print..."), this);
    printAct->setShortcut(tr("Ctrl+P"));
    printAct->setEnabled(false);
    connect(printAct, SIGNAL(triggered()), this, SLOT(print()));

    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcut(tr("Ctrl+X"));
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

    zoomInAct = new QAction(tr("Zoom &In (25%)"), this);
    zoomInAct->setShortcut(tr("Ctrl++"));
    zoomInAct->setEnabled(false);
    connect(zoomInAct, SIGNAL(triggered()), this, SLOT(zoomIn()));

    zoomOutAct = new QAction(tr("Zoom &Out (25%)"), this);
    zoomOutAct->setShortcut(tr("Ctrl+-"));
    zoomOutAct->setEnabled(false);
    connect(zoomOutAct, SIGNAL(triggered()), this, SLOT(zoomOut()));

    normalSizeAct = new QAction(tr("&Normal Size"), this);
    normalSizeAct->setShortcut(tr("Ctrl+N"));
    normalSizeAct->setEnabled(false);
    connect(normalSizeAct, SIGNAL(triggered()), this, SLOT(normalSize()));

    startAct = new QAction(tr("&Start"), this);
    startAct->setShortcut(tr("Ctrl+S"));
    startAct->setEnabled(false);
    startAct->setStatusTip(tr("Start the active contour."));
    connect(startAct, SIGNAL(triggered()), this, SLOT(start()));

    stopAct = new QAction(tr("S&top"), this);
    stopAct->setShortcut(tr("Ctrl+T"));
    stopAct->setEnabled(false);
    connect(stopAct, SIGNAL(triggered()), this, SLOT(stop()));

    evaluateAct = new QAction(tr("E&valuate"), this);
    evaluateAct->setStatusTip(tr("Compute the modified Hausdorff distance."));
    evaluateAct->setShortcut(tr("Ctrl+V"));
    evaluateAct->setEnabled(true);
    connect(evaluateAct, SIGNAL(triggered()), this, SLOT(evaluation()));

    settingsAct = new QAction(tr("S&ettings"), this);
    settingsAct->setStatusTip(tr("Image preprocessing and active contour initialization."));
    settingsAct->setShortcut(tr("Ctrl+E"));
    settingsAct->setEnabled(true);
    connect(settingsAct, SIGNAL(triggered()), this, SLOT(settings()));

    menuBar()->addSeparator();

    aboutAct = new QAction(tr("&About"), this);
    aboutAct->setStatusTip(tr("Information, license and home page."));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    languageAct = new QAction(tr("&Language"), this);
    languageAct->setStatusTip(tr("Choose the application language."));
    connect(languageAct, SIGNAL(triggered()), this, SLOT(language()));

    docAct = new QAction(tr("&Documentation"), this);
    docAct->setStatusTip(tr("Online developer's documentation."));
    connect(docAct, SIGNAL(triggered()), this, SLOT(doc()));

    aboutQtAct = new QAction(tr("About &Qt"), this);
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

// Creation du menu de la fenêtre principale
void ImageViewer::createMenus()
{
    fileMenu = new QMenu(tr("&File"), this);
    fileMenu->addAction(openAct);

    separatorAct = fileMenu->addSeparator();
    for( int i = 0; i < MaxRecentFiles; ++i )
    {
        fileMenu->addAction(recentFileActs[i]);
    }

    fileMenu->addAction(deleteAct);

    fileMenu->addSeparator();

    fileMenu->addAction(saveAct);
    fileMenu->addAction(printAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    updateRecentFileActions();

    viewMenu = new QMenu(tr("&View"), this);
    viewMenu->addAction(zoomInAct);
    viewMenu->addAction(zoomOutAct);
    viewMenu->addAction(normalSizeAct);

    segmentationMenu = new QMenu(tr("&Segmentation"), this);
    segmentationMenu->addAction(startAct);
    segmentationMenu->addAction(stopAct);
    segmentationMenu->addAction(evaluateAct);
    segmentationMenu->addAction(settingsAct);

    helpMenu = new QMenu(tr("&Help"), this);
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(languageAct);
    helpMenu->addAction(docAct);
    //helpMenu->addAction(aboutQtAct);

    menuBar()->addMenu(fileMenu);
    menuBar()->addMenu(viewMenu);
    menuBar()->addMenu(segmentationMenu);
    menuBar()->addMenu(helpMenu);
}

// Fonction d'origine, modifiée pour le changement d'echelle de l'image
void ImageViewer::scaleImage(double factor)
{
    Q_ASSERT(imageLabel->pixmap());
    scaleFactor *= factor;
    imageLabel->resize(scaleFactor * imageLabel->pixmap()->size());

    // ajout de code, pour récuperer les infos lors d'un zoom pour la segmentation
    /////

    adjustScrollBar(scrollArea->horizontalScrollBar(), factor);
    adjustScrollBar(scrollArea->verticalScrollBar(), factor);

    zoomInAct->setEnabled(scaleFactor < 3.0);
    zoomOutAct->setEnabled(scaleFactor > 0.333);
}

void ImageViewer::adjustScrollBar(QScrollBar *scrollBar, double factor)
{
    scrollBar->setValue(int(factor * scrollBar->value()
                            + ((factor - 1) * scrollBar->pageStep()/2)));
}



/////////////////   Fin des fonctions d'origine de l'exemple ImageViewer de Qt  ///////////



////////  Fonctions appelées pour la création de la fenêtre de configuration  //////

// Création du layout pour l'onglet algorithm de la fenêtre de configuration
QVBoxLayout *ImageViewer::algorithm()
{
    // création de 3 groupbox pour l'onglet algorithm

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////

    QGroupBox* externalspeed_groupbox = new QGroupBox(tr("Cycle 1 : data dependant evolution"));

    Na_spin = new QSpinBox;
    Na_spin->setSingleStep(1);
    Na_spin->setMinimum(1);
    Na_spin->setMaximum(999);
    Na_spin->setSuffix(tr(" iterations"));
    Na_spin->setToolTip(tr("iterations in the cycle 1, active contour penetrability"));
    QFormLayout *Na_layout = new QFormLayout;
    Na_layout->addRow("Na =", Na_spin);
    if( operating_system == 2 )
    {
        Na_layout->setFormAlignment(Qt::AlignLeft);
    }

    klength_gradient_spin = new QSpinBox;
    klength_gradient_spin->setSingleStep(2);
    klength_gradient_spin->setMinimum(1);
    klength_gradient_spin->setMaximum(499);

    chanvese_radio = new QRadioButton(tr("Chan-Vese model"));
    chanvese_radio->setToolTip(tr("region-based model for bimodal images"));

    lambda_out_spin = new QSpinBox;
    lambda_out_spin->setSingleStep(1);
    lambda_out_spin->setMinimum(0);
    lambda_out_spin->setMaximum(100000);
    lambda_out_spin->setToolTip(tr("weight of the outside homogeneity criterion"));

    lambda_in_spin = new QSpinBox;
    lambda_in_spin->setSingleStep(1);
    lambda_in_spin->setMinimum(0);
    lambda_in_spin->setMaximum(100000);
    lambda_in_spin->setToolTip(tr("weight of the inside homogeneity criterion"));

    QFormLayout* lambda_layout = new QFormLayout;
    QSettings settings("Bessy", "Ofeli");
    outside_combo = settings.value("Settings/Display/outside_combo", 2).toInt();
    inside_combo = settings.value("Settings/Display/inside_combo", 0).toInt();

    Rout_selected1 = (unsigned char)(settings.value("Settings/Display/Rout_selected", 128).toInt());
    Gout_selected1 = (unsigned char)(settings.value("Settings/Display/Gout_selected", 0).toInt());
    Bout_selected1 = (unsigned char)(settings.value("Settings/Display/Bout_selected", 255).toInt());
    Rin_selected1 = (unsigned char)(settings.value("Settings/Display/Rin_selected", 255).toInt());
    Gin_selected1 = (unsigned char)(settings.value("Settings/Display/Gin_selected", 128).toInt());
    Bin_selected1 = (unsigned char)(settings.value("Settings/Display/Bin_selected", 0).toInt());

    if( outside_combo == 8 )
    {
        Rout1 = Rout_selected1;
        Gout1 = Gout_selected1;
        Bout1 = Bout_selected1;
    }
    else
    {
        get_color(outside_combo,Rout1,Gout1,Bout1);
    }

    if( inside_combo == 8 )
    {
        Rin1 = Rin_selected1;
        Gin1 = Gin_selected1;
        Bin1 = Bin_selected1;
    }
    else
    {
        get_color(inside_combo,Rin1,Gin1,Bin1);
    }

    QColor RGBout_list(Rout1,Gout1,Bout1);
    QColor RGBin_list(Rin1,Gin1,Bin1);
    lambda_layout->addRow("<font color="+RGBout_list.name()+">"+"λout"+"<font color=black>"+" =", lambda_out_spin);
    lambda_layout->addRow("<font color="+RGBin_list.name()+">"+"λin"+"<font color=black>"+" =", lambda_in_spin);
    if( operating_system == 2 )
    {
        lambda_layout->setFormAlignment(Qt::AlignLeft);
    }

    QVBoxLayout* chanvese_layout = new QVBoxLayout;
    chanvese_layout->addWidget(chanvese_radio);
    chanvese_layout->addLayout(lambda_layout);

    geodesic_radio = new QRadioButton(tr("geodesic model"));
    geodesic_radio->setToolTip(tr("edge-based model for smoothed multimodal images"));

    klength_gradient_spin = new QSpinBox;
    klength_gradient_spin->setSingleStep(2);
    klength_gradient_spin->setMinimum(1);
    klength_gradient_spin->setMaximum(499);
    klength_gradient_spin->setSuffix("²");
    klength_gradient_spin->setToolTip(tr("morphological gradient structuring element size"));
    QFormLayout *gradient_layout = new QFormLayout;
    gradient_layout->addRow(tr("SE size ="), klength_gradient_spin);
    if( operating_system == 2 )
    {
        gradient_layout->setFormAlignment(Qt::AlignLeft);
    }
    connect(klength_gradient_spin,SIGNAL(valueChanged(int)),this,SLOT(filtering_visu()));

    QVBoxLayout* geodesic_layout = new QVBoxLayout;
    geodesic_layout->addWidget(geodesic_radio);
    geodesic_layout->addLayout(gradient_layout);

    QHBoxLayout* speed_layout = new QHBoxLayout;
    speed_layout->addLayout(chanvese_layout);
    speed_layout->addLayout(geodesic_layout);

    yuv_groupbox = new QGroupBox(tr("normalized (Y,U,V) color space weighting"));
    yuv_groupbox->setFlat(true);

    Yspin = new QSpinBox;
    Yspin->setSingleStep(1);
    Yspin->setMinimum(0);
    Yspin->setMaximum(100000);
    Yspin->setToolTip(tr("luminance weight"));

    Uspin = new QSpinBox;
    Uspin->setSingleStep(1);
    Uspin->setMinimum(0);
    Uspin->setMaximum(100000);
    Uspin->setToolTip(tr("chrominance weight"));

    Vspin = new QSpinBox;
    Vspin->setSingleStep(1);
    Vspin->setMinimum(0);
    Vspin->setMaximum(100000);
    Vspin->setToolTip(tr("chrominance weight"));

    connect(Yspin,SIGNAL(valueChanged(int)),this,SLOT(filtering_visu()));
    connect(Uspin,SIGNAL(valueChanged(int)),this,SLOT(filtering_visu()));
    connect(Vspin,SIGNAL(valueChanged(int)),this,SLOT(filtering_visu()));

    QFormLayout* yuv_layout = new QFormLayout;
    yuv_layout->addRow(tr("Y weight ="), Yspin);
    yuv_layout->addRow(tr("U weight ="), Uspin);
    yuv_layout->addRow(tr("V weight ="), Vspin);
    if( operating_system == 2 )
    {
        yuv_layout->setFormAlignment(Qt::AlignLeft);
    }
    yuv_groupbox->setLayout(yuv_layout);

    connect(chanvese_radio,SIGNAL(clicked()),this,SLOT(filtering_visu()));
    connect(geodesic_radio,SIGNAL(clicked()),this,SLOT(filtering_visu()));

    QVBoxLayout* externalspeed_layout = new QVBoxLayout;

    externalspeed_layout->addLayout(Na_layout);
    externalspeed_layout->addLayout(speed_layout);
    externalspeed_layout->addWidget(yuv_groupbox);
    externalspeed_groupbox->setLayout(externalspeed_layout);

    ////////////////////////////////////////////////////////////////////////////////////////////////

    internalspeed_groupbox = new QGroupBox(tr("Cycle 2 - smoothing via gaussian filtring"));
    internalspeed_groupbox->setCheckable(true);
    internalspeed_groupbox->setChecked(true);

    Ns_spin = new QSpinBox;
    Ns_spin->setSingleStep(1);
    Ns_spin->setMinimum(1);
    Ns_spin->setMaximum(999);
    Ns_spin->setSuffix(tr(" iterations"));
    Ns_spin->setToolTip(tr("iterations in the cycle 2, active contour regularization"));
    QFormLayout* Ns_layout = new QFormLayout;
    Ns_layout->addRow("Ns =", Ns_spin);
    if( operating_system == 2 )
    {
        Ns_layout->setFormAlignment(Qt::AlignLeft);
    }

    klength_spin = new QSpinBox;
    klength_spin->setSingleStep(2);
    klength_spin->setMinimum(1);
    klength_spin->setMaximum(499);
    klength_spin->setToolTip(tr("gaussian kernel size = Ng × Ng "));
    std_spin = new QDoubleSpinBox;
    std_spin->setSingleStep(0.1);
    std_spin->setMinimum(0.0);
    std_spin->setMaximum(1000000.0);
    std_spin->setToolTip(tr("standard deviation of the gaussian kernel"));

    QFormLayout* internalspeed_layout = new QFormLayout;
    internalspeed_layout->addRow("Ns =", Ns_spin);
    internalspeed_layout->addRow("Ng =", klength_spin);
    internalspeed_layout->addRow("σ =", std_spin);
    if( operating_system == 2 )
    {
        internalspeed_layout->setFormAlignment(Qt::AlignLeft);
    }
    internalspeed_groupbox->setLayout(internalspeed_layout);

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////

    QVBoxLayout* algorithm_layout = new QVBoxLayout;
    algorithm_layout->addWidget(externalspeed_groupbox);
    algorithm_layout->addWidget(internalspeed_groupbox);
    algorithm_layout->addStretch(1);

    return algorithm_layout;
}

// Création du layout pour l'onglet initialization de la fenêtre de configuration
QVBoxLayout* ImageViewer::initialization()
{
    open_phi_button = new QPushButton(tr("Open ϕ(t=0)"));
    open_phi_button->setToolTip(tr("or drag and drop an image of ϕ(t=0)"));
    save_phi_button = new QPushButton(tr("Save ϕ(t=0)"));
    connect(open_phi_button,SIGNAL(clicked()),this,SLOT(openFilenamePhi()));
    connect(save_phi_button,SIGNAL(clicked()),this,SLOT(save_phi()));
    open_phi_button->setEnabled(false);
    save_phi_button->setEnabled(false);


    QHBoxLayout* openSavePhi= new QHBoxLayout;
    openSavePhi->addWidget(open_phi_button);
    openSavePhi->addWidget(save_phi_button);

    // création des 4 groupbox pour l'onglet initialization

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////

    QGroupBox* shape_groupbox = new QGroupBox(tr("Shape"));
    shape_groupbox->setFlat(true);

    rectangle_radio = new QRadioButton(tr("rectangle"));
    rectangle_radio->setToolTip(tr("or click on the middle mouse button when the cursor is in the image"));
    ellipse_radio = new QRadioButton(tr("ellipse"));
    ellipse_radio->setToolTip(tr("or click on the middle mouse button when the cursor is in the image"));
    connect(rectangle_radio,SIGNAL(clicked()),this,SLOT(shape_visu()));
    connect(ellipse_radio,SIGNAL(clicked()),this,SLOT(shape_visu()));

    QHBoxLayout* shape_layout = new QHBoxLayout;
    shape_layout->addWidget(rectangle_radio);
    shape_layout->addWidget(ellipse_radio);
    shape_groupbox->setLayout(shape_layout);

    ////////////////////////////////////////////////////////////////////////////////////////////////

    QGroupBox* size_groupbox = new QGroupBox(tr("Size"));
    size_groupbox ->setToolTip(tr("or roll the mouse wheel when the cursor is in the image"));

    width_shape_spin = new QSpinBox;
    width_shape_spin->setSingleStep(15);
    width_shape_spin->setMinimum(0);
    width_shape_spin->setMaximum(1000);
    width_shape_spin->setSuffix(tr(" % image width"));

    QFormLayout* width_spin_layout = new QFormLayout;
    width_spin_layout->addRow(tr("width ="), width_shape_spin);
    if( operating_system == 2 )
    {
        width_spin_layout->setFormAlignment(Qt::AlignLeft);
    }

    width_slider = new QSlider(Qt::Horizontal, this);
    width_slider->setTickPosition(QSlider::TicksAbove);
    width_slider->setMinimum(0);
    width_slider->setMaximum(150);
    width_slider->setTickInterval(25);
    width_slider->setSingleStep(15);

    connect(width_shape_spin,SIGNAL(valueChanged(int)),this,SLOT(shape_visu(int)));
    connect(width_slider,SIGNAL(valueChanged(int)),width_shape_spin,SLOT(setValue(int)));
    connect(width_shape_spin,SIGNAL(valueChanged(int)),width_slider,SLOT(setValue(int)));

    height_shape_spin = new QSpinBox;
    height_shape_spin->setSingleStep(15);
    height_shape_spin->setMinimum(0);
    height_shape_spin->setMaximum(1000);
    height_shape_spin->setSuffix(tr((" % image height")));

    QFormLayout* height_spin_layout = new QFormLayout;
    height_spin_layout->addRow(tr("height ="), height_shape_spin);
    if( operating_system == 2 )
    {
        height_spin_layout->setFormAlignment(Qt::AlignLeft);
    }

    height_slider = new QSlider(Qt::Horizontal, this);
    height_slider->setTickPosition(QSlider::TicksAbove);
    height_slider->setMinimum(0);
    height_slider->setMaximum(150);
    height_slider->setTickInterval(25);
    height_slider->setSingleStep(15);

    connect(height_shape_spin,SIGNAL(valueChanged(int)),this,SLOT(shape_visu(int)));
    connect(height_slider,SIGNAL(valueChanged(int)),height_shape_spin,SLOT(setValue(int)));
    connect(height_shape_spin,SIGNAL(valueChanged(int)),height_slider,SLOT(setValue(int)));

    QVBoxLayout* size_layout = new QVBoxLayout;
    size_layout->addLayout(width_spin_layout);
    size_layout->addWidget(width_slider);
    size_layout->addLayout(height_spin_layout);
    size_layout->addWidget(height_slider);
    size_groupbox->setLayout(size_layout);

    ////////////////////////////////////////////////////////////////////////////////////////////////

    QGroupBox* position_groupbox = new QGroupBox(tr("Position (x,y)"));
    position_groupbox->setToolTip(tr("or move the mouse cursor in the image"));

    abscissa_spin = new QSpinBox;
    abscissa_spin->setSingleStep(15);
    abscissa_spin->setMinimum(-500);
    abscissa_spin->setMaximum(500);
    abscissa_spin->setSuffix(tr(" % image width"));

    QFormLayout* abscissa_spin_layout = new QFormLayout;
    abscissa_spin_layout->addRow("x = Xo +", abscissa_spin);
    if( operating_system == 2 )
    {
        abscissa_spin_layout->setFormAlignment(Qt::AlignLeft);
    }

    abscissa_slider = new QSlider(Qt::Horizontal, this);
    abscissa_slider->setTickPosition(QSlider::TicksAbove);
    abscissa_slider->setMinimum(-75);
    abscissa_slider->setMaximum(75);
    abscissa_slider->setTickInterval(25);
    abscissa_slider->setSingleStep(15);

    connect(abscissa_spin,SIGNAL(valueChanged(int)),this,SLOT(shape_visu(int)));
    connect(abscissa_slider,SIGNAL(valueChanged(int)),abscissa_spin,SLOT(setValue(int)));
    connect(abscissa_spin,SIGNAL(valueChanged(int)),abscissa_slider,SLOT(setValue(int)));

    ordinate_spin = new QSpinBox;
    ordinate_spin->setSingleStep(15);
    ordinate_spin->setMinimum(-500);
    ordinate_spin->setMaximum(500);
    ordinate_spin->setSuffix(tr(" % image height"));

    QFormLayout* ordinate_spin_layout = new QFormLayout;
    ordinate_spin_layout->addRow("y = Yo +", ordinate_spin);
    if( operating_system == 2 )
    {
        ordinate_spin_layout->setFormAlignment(Qt::AlignLeft);
    }

    ordinate_slider = new QSlider(Qt::Horizontal, this);
    ordinate_slider->setTickPosition(QSlider::TicksAbove);
    ordinate_slider->setMinimum(-75);
    ordinate_slider->setMaximum(75);
    ordinate_slider->setTickInterval(25);
    ordinate_slider->setSingleStep(15);

    connect(ordinate_spin,SIGNAL(valueChanged(int)),this,SLOT(shape_visu(int)));
    connect(ordinate_slider,SIGNAL(valueChanged(int)),ordinate_spin,SLOT(setValue(int)));
    connect(ordinate_spin,SIGNAL(valueChanged(int)),ordinate_slider,SLOT(setValue(int)));

    QVBoxLayout* position_layout = new QVBoxLayout;
    position_layout->addLayout(abscissa_spin_layout);
    position_layout->addWidget(abscissa_slider);
    position_layout->addLayout(ordinate_spin_layout);
    position_layout->addWidget(ordinate_slider);
    position_groupbox->setLayout(position_layout);

    ////////////////////////////////////////////////////////////////////////////////////////////////

    QGroupBox* modify_groupbox = new QGroupBox(tr("Active contour modification"));
    modify_groupbox->setFlat(true);

    add_button = new QPushButton(tr("Add"));
    add_button->setToolTip(tr("or click on the left mouse button when the cursor is in the image"));
    connect(add_button,SIGNAL(clicked()),this,SLOT(add_visu()));

    subtract_button = new QPushButton(tr("Subtract"));
    subtract_button->setToolTip(tr("or click on the right mouse button when the cursor is in the image"));
    connect(subtract_button,SIGNAL(clicked()),this,SLOT(subtract_visu()));

    clean_button = new QPushButton(tr("Clean"));
    connect(clean_button,SIGNAL(clicked()),this,SLOT(clean_phi_visu()));

    QHBoxLayout* modify_layout = new QHBoxLayout;
    modify_layout->addWidget(clean_button);
    modify_layout->addWidget(subtract_button);
    modify_layout->addWidget(add_button);
    modify_groupbox->setLayout(modify_layout);

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////

    QVBoxLayout* initialization_layout = new QVBoxLayout;
    initialization_layout->addLayout(openSavePhi);
    initialization_layout->addWidget(shape_groupbox);
    initialization_layout->addWidget(size_groupbox);
    initialization_layout->addWidget(position_groupbox);
    initialization_layout->addWidget(modify_groupbox);
    initialization_layout->addStretch(1);

    return initialization_layout;

}

// Création du layout pour l'onglet preprocessing de la fenêtre de configuration
QGroupBox* ImageViewer::preprocessing()
{
    // création de groupbox pour l'onglet noise generators

    gaussian_noise_groupbox = new QGroupBox(tr("Gaussian white noise"));
    gaussian_noise_groupbox->setCheckable(true);
    gaussian_noise_groupbox->setChecked(false);

    std_noise_spin = new QDoubleSpinBox;
    std_noise_spin->setSingleStep(5.0);
    std_noise_spin->setMinimum(0.0);
    std_noise_spin->setMaximum(10000.0);
    std_noise_spin->setToolTip(tr("standard deviation"));

    QFormLayout* gaussian_noise_layout = new QFormLayout;
    gaussian_noise_layout->addRow("σ =", std_noise_spin);

    salt_noise_groupbox = new QGroupBox(tr("Impulsional noise (salt and pepper)"));
    salt_noise_groupbox->setCheckable(true);
    salt_noise_groupbox->setChecked(false);

    proba_noise_spin = new QDoubleSpinBox;
    proba_noise_spin->setSingleStep(1.0);
    proba_noise_spin->setMinimum(0.0);
    proba_noise_spin->setMaximum(100.0);
    proba_noise_spin->setSuffix(" %");
    proba_noise_spin->setToolTip(tr("impulsional noise probability for each pixel"));

    QFormLayout* salt_noise_layout = new QFormLayout;
    salt_noise_layout->addRow(tr("d ="), proba_noise_spin);

    speckle_noise_groupbox = new QGroupBox(tr("Speckle noise"));
    speckle_noise_groupbox->setCheckable(true);
    speckle_noise_groupbox->setChecked(false);

    std_speckle_noise_spin = new QDoubleSpinBox;
    std_speckle_noise_spin->setSingleStep(0.02);
    std_speckle_noise_spin->setMinimum(0.0);
    std_speckle_noise_spin->setMaximum(1000.0);
    std_speckle_noise_spin->setToolTip(tr("standard deviation"));

    QFormLayout* speckle_noise_layout = new QFormLayout;
    speckle_noise_layout->addRow("σ =", std_speckle_noise_spin);

    if( operating_system == 2 )
    {
        gaussian_noise_layout->setFormAlignment(Qt::AlignLeft);
        salt_noise_layout->setFormAlignment(Qt::AlignLeft);
        speckle_noise_layout->setFormAlignment(Qt::AlignLeft);
    }

    gaussian_noise_groupbox->setLayout(gaussian_noise_layout);
    salt_noise_groupbox->setLayout(salt_noise_layout);
    speckle_noise_groupbox->setLayout(speckle_noise_layout);

    connect(gaussian_noise_groupbox,SIGNAL(clicked()),this,SLOT(filtering_visu()));
    connect(salt_noise_groupbox,SIGNAL(clicked()),this,SLOT(filtering_visu()));
    connect(speckle_noise_groupbox,SIGNAL(clicked()),this,SLOT(filtering_visu()));

    connect(std_noise_spin,SIGNAL(valueChanged(double)),this,SLOT(filtering_visu()));
    connect(proba_noise_spin,SIGNAL(valueChanged(double)),this,SLOT(filtering_visu()));
    connect(std_speckle_noise_spin,SIGNAL(valueChanged(double)),this,SLOT(filtering_visu()));


    QVBoxLayout* noise_layout = new QVBoxLayout;
    noise_layout->addWidget(gaussian_noise_groupbox);
    noise_layout->addWidget(salt_noise_groupbox);
    noise_layout->addWidget(speckle_noise_groupbox);
    noise_layout->addStretch(1);

    // création de groupbox pour l'onglet filters

    mean_groupbox = new QGroupBox(tr("Mean filter"));
    mean_groupbox->setCheckable(true);
    mean_groupbox->setChecked(false);

    klength_mean_spin = new QSpinBox;
    klength_mean_spin->setSingleStep(2);
    klength_mean_spin->setMinimum(1);
    klength_mean_spin->setMaximum(499);
    klength_mean_spin->setSuffix("²");

    gaussian_groupbox = new QGroupBox(tr("Gaussian filter"));
    gaussian_groupbox->setCheckable(true);
    gaussian_groupbox->setChecked(false);

    klength_gaussian_spin = new QSpinBox;
    klength_gaussian_spin->setSingleStep(2);
    klength_gaussian_spin->setMinimum(1);
    klength_gaussian_spin->setMaximum(499);
    klength_gaussian_spin->setSuffix("²");
    std_filter_spin = new QDoubleSpinBox;
    std_filter_spin->setSingleStep(0.1);
    std_filter_spin->setMinimum(0.0);
    std_filter_spin->setMaximum(1000000.0);
    std_filter_spin->setToolTip(tr("standard deviation"));


    median_groupbox = new QGroupBox(tr("Median filter"));
    median_groupbox->setCheckable(true);
    median_groupbox->setChecked(false);

    klength_median_spin = new QSpinBox;
    klength_median_spin->setSingleStep(2);
    klength_median_spin->setMinimum(1);
    klength_median_spin->setMaximum(499);
    klength_median_spin->setSuffix("²");

    complex_radio1= new QRadioButton("O(r log r)×O(n)");
    complex_radio2= new QRadioButton("O(1)×O(n)");

    connect(complex_radio1,SIGNAL(clicked()),this,SLOT(filtering_visu()));
    connect(complex_radio2,SIGNAL(clicked()),this,SLOT(filtering_visu()));

    aniso_groupbox = new QGroupBox(tr("Perona-Malik anisotropic diffusion"));
    aniso_groupbox->setCheckable(true);
    aniso_groupbox->setChecked(false);

    aniso1_radio = new QRadioButton("g(∇I) = exp(-(|∇I|/κ)²)");
    aniso2_radio = new QRadioButton("g(∇I) = 1/(1+(1+(|∇I|/κ)²)");

    iteration_filter_spin = new QSpinBox;
    iteration_filter_spin->setSingleStep(1);
    iteration_filter_spin->setMinimum(0);
    iteration_filter_spin->setMaximum(5000);

    lambda_spin = new QDoubleSpinBox;
    lambda_spin->setSingleStep(0.01);
    lambda_spin->setMinimum(0.0);
    lambda_spin->setMaximum(1.0/4.0);


    kappa_spin = new QDoubleSpinBox;
    kappa_spin->setSingleStep(1.0);
    kappa_spin->setMinimum(0.0);
    kappa_spin->setMaximum(10000.0);


    open_groupbox = new QGroupBox(tr("Opening"));
    open_groupbox->setCheckable(true);
    open_groupbox->setChecked(false);

    klength_open_spin = new QSpinBox;
    klength_open_spin->setSingleStep(2);
    klength_open_spin->setMinimum(1);
    klength_open_spin->setMaximum(499);
    klength_open_spin->setSuffix("²");
    klength_open_spin->setToolTip(tr("the structuring element shape is a square and its origin is the center of the square"));


    close_groupbox = new QGroupBox(tr("Closing"));
    close_groupbox->setCheckable(true);
    close_groupbox->setChecked(false);

    klength_close_spin = new QSpinBox;
    klength_close_spin->setSingleStep(2);
    klength_close_spin->setMinimum(1);
    klength_close_spin->setMaximum(499);
    klength_close_spin->setSuffix("²");
    klength_close_spin->setToolTip(tr("the structuring element shape is a square and its origin is the center of the square"));

    tophat_groupbox = new QGroupBox(tr("Top-hat transform"));
    tophat_groupbox->setCheckable(true);
    tophat_groupbox->setChecked(false);

    whitetophat_radio = new QRadioButton(tr("white top-hat"));
    whitetophat_radio->setToolTip(tr("difference between the input image the opened"));
    blacktophat_radio = new QRadioButton(tr("black top-hat"));
    blacktophat_radio->setToolTip(tr("difference between the closed and the input image"));
    klength_tophat_spin = new QSpinBox;
    klength_tophat_spin->setSingleStep(2);
    klength_tophat_spin->setMinimum(1);
    klength_tophat_spin->setMaximum(499);
    klength_tophat_spin->setSuffix("²");
    klength_tophat_spin->setToolTip(tr("the structuring element shape is a square and its origin is the center of the square"));

    algo_groupbox = new QGroupBox(tr("Algorithm"));
    complex1_morpho_radio = new QRadioButton(tr("naïve algorithm in O(r)×O(n)"));
    complex2_morpho_radio = new QRadioButton(tr("Perreault's algorithm in O(1)×O(n)"));
    QVBoxLayout* algo_layout = new QVBoxLayout();
    algo_layout->addWidget(complex1_morpho_radio);
    algo_layout->addWidget(complex2_morpho_radio);

    connect(complex1_morpho_radio,SIGNAL(clicked()),this,SLOT(filtering_visu()));
    connect(complex2_morpho_radio,SIGNAL(clicked()),this,SLOT(filtering_visu()));

    connect(median_groupbox,SIGNAL(clicked()),this,SLOT(filtering_visu()));
    connect(klength_median_spin,SIGNAL(valueChanged(int)),this,SLOT(filtering_visu()));

    connect(mean_groupbox,SIGNAL(clicked()),this,SLOT(filtering_visu()));
    connect(klength_mean_spin,SIGNAL(valueChanged(int)),this,SLOT(filtering_visu()));

    connect(gaussian_groupbox,SIGNAL(clicked()),this,SLOT(filtering_visu()));
    connect(klength_gaussian_spin,SIGNAL(valueChanged(int)),this,SLOT(filtering_visu()));
    connect(std_filter_spin,SIGNAL(valueChanged(double)),this,SLOT(filtering_visu()));

    connect(aniso_groupbox,SIGNAL(clicked()),this,SLOT(filtering_visu()));
    connect(aniso1_radio,SIGNAL(clicked()),this,SLOT(filtering_visu()));
    connect(aniso2_radio,SIGNAL(clicked()),this,SLOT(filtering_visu()));
    connect(iteration_filter_spin,SIGNAL(valueChanged(int)),this,SLOT(filtering_visu()));
    connect(lambda_spin,SIGNAL(valueChanged(double)),this,SLOT(filtering_visu()));
    connect(kappa_spin,SIGNAL(valueChanged(double)),this,SLOT(filtering_visu()));

    connect(open_groupbox,SIGNAL(clicked()),this,SLOT(filtering_visu()));
    connect(klength_open_spin,SIGNAL(valueChanged(int)),this,SLOT(filtering_visu()));

    connect(close_groupbox,SIGNAL(clicked()),this,SLOT(filtering_visu()));
    connect(klength_close_spin,SIGNAL(valueChanged(int)),this,SLOT(filtering_visu()));

    connect(tophat_groupbox,SIGNAL(clicked()),this,SLOT(filtering_visu()));
    connect(whitetophat_radio,SIGNAL(clicked()),this,SLOT(filtering_visu()));
    connect(blacktophat_radio,SIGNAL(clicked()),this,SLOT(filtering_visu()));
    connect(klength_tophat_spin,SIGNAL(valueChanged(int)),this,SLOT(filtering_visu()));

    QFormLayout* mean_layout = new QFormLayout;
    mean_layout->addRow(tr("kernel size ="), klength_mean_spin);

    QFormLayout* gaussian_layout = new QFormLayout;
    gaussian_layout->addRow(tr("kernel size ="), klength_gaussian_spin);
    gaussian_layout->addRow("σ =", std_filter_spin);

    QFormLayout* median_layout = new QFormLayout;
    median_layout->addRow(tr("kernel size ="), klength_median_spin);
    median_layout->addRow(tr("quick sort algorithm"), complex_radio1);
    median_layout->addRow(tr("Perreault's algorithm"), complex_radio2);

    QFormLayout* aniso_layout = new QFormLayout;
    aniso_layout->addRow(tr("iterations ="), iteration_filter_spin);
    aniso_layout->addRow("λ =", lambda_spin);
    aniso_layout->addRow("κ =", kappa_spin);
    aniso_layout->addRow(tr("function 1 :"), aniso1_radio);
    aniso_layout->addRow(tr("function 2 :"), aniso2_radio);

    QFormLayout* open_layout = new QFormLayout;
    open_layout->addRow(tr("SE size ="), klength_open_spin);

    QFormLayout* close_layout = new QFormLayout;
    close_layout->addRow(tr("SE size ="), klength_close_spin);

    QFormLayout* tophat_layout = new QFormLayout;
    tophat_layout->addRow(" ", whitetophat_radio);
    tophat_layout->addRow(" ", blacktophat_radio);
    tophat_layout->addRow(tr("SE size ="), klength_tophat_spin);

    if( operating_system == 2 )
    {
        median_layout->setFormAlignment(Qt::AlignLeft);
        mean_layout->setFormAlignment(Qt::AlignLeft);
        gaussian_layout->setFormAlignment(Qt::AlignLeft);
        aniso_layout->setFormAlignment(Qt::AlignLeft);
        open_layout->setFormAlignment(Qt::AlignLeft);
        close_layout->setFormAlignment(Qt::AlignLeft);
        tophat_layout->setFormAlignment(Qt::AlignLeft);
    }

    mean_groupbox->setLayout(mean_layout);
    gaussian_groupbox->setLayout(gaussian_layout);
    median_groupbox->setLayout(median_layout);
    aniso_groupbox->setLayout(aniso_layout);
    open_groupbox->setLayout(open_layout);
    close_groupbox->setLayout(close_layout);
    tophat_groupbox->setLayout(tophat_layout);
    algo_groupbox->setLayout(algo_layout);

    QVBoxLayout* filter_layout_linear = new QVBoxLayout;
    filter_layout_linear->addWidget(mean_groupbox);
    filter_layout_linear->addWidget(gaussian_groupbox);

    QVBoxLayout* filter_layout_edge_preserv = new QVBoxLayout;
    filter_layout_edge_preserv->addWidget(median_groupbox);
    filter_layout_edge_preserv->addWidget(aniso_groupbox);

    QVBoxLayout* filter_layout_mm = new QVBoxLayout;
    filter_layout_mm->addWidget(open_groupbox);
    filter_layout_mm->addWidget(close_groupbox);
    filter_layout_mm->addWidget(tophat_groupbox);
    filter_layout_mm->addWidget(algo_groupbox);


    ////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////

    // création du widget onglets, visible a gauche dans la fenêtre de configuration
    page3 = new QTabWidget(settings_window);

    // création de widgets correspondant a chaque page ou a chaque onglet du QTabWidget onglets
    QWidget* page_noise = new QWidget;

    QWidget* page_filter_iso = new QWidget;
    QWidget* page_filter_ansio = new QWidget;
    QWidget* page_filter_morpho = new QWidget;

    // widget contenant des layouts
    noise_layout->addStretch(1);
    page_noise->setLayout(noise_layout);
    filter_layout_linear->addStretch(1);
    page_filter_iso->setLayout(filter_layout_linear);
    filter_layout_edge_preserv->addStretch(1);
    page_filter_ansio->setLayout(filter_layout_edge_preserv);
    filter_layout_mm->addStretch(1);
    page_filter_morpho->setLayout(filter_layout_mm);

    // création du widget onglets, visible a gauche dans la fenêtre de configuration
    QTabWidget* tab_filter = new QTabWidget(settings_window);
    tab_filter->addTab(page_filter_iso, tr("Linear"));
    tab_filter->addTab(page_filter_ansio, tr("Edge preserving"));
    tab_filter->addTab(page_filter_morpho, tr("Math morpho"));

    // ajout des 2 widgets correspondant aux 2 onglets (aux 2 pages) dans le QTabWidget avec un titre pour chaque onglet
    page3->addTab(page_noise, tr("Noise generators"));
    page3->addTab(tab_filter, tr("Filters"));



    preprocessing_groupbox = new QGroupBox(tr("Preprocessing"));
    preprocessing_groupbox->setCheckable(true);
    preprocessing_groupbox->setChecked(false);

    connect(preprocessing_groupbox,SIGNAL(clicked()),this,SLOT(filtering_visu()));

    time_filt=new QLabel;
    time_filt->setText(tr("time = "));

    QGroupBox* time_filt_groupbox=new QGroupBox(tr("Processing time"));
    QVBoxLayout* elapsed_filt_layout = new QVBoxLayout;
    elapsed_filt_layout->addWidget(time_filt);
    time_filt_groupbox->setLayout(elapsed_filt_layout);


    QVBoxLayout* vbox = new QVBoxLayout;
    vbox->addWidget(page3);
    vbox->addWidget(time_filt_groupbox);
    //vbox->addStretch(1);
    preprocessing_groupbox->setLayout(vbox);

    return preprocessing_groupbox;

}

// Création du layout pour l'onglet display de la fenêtre de configuration
QVBoxLayout* ImageViewer::display()
{
    // création de 3 groupbox pour l'onglet display

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////

    QGroupBox* size_groupbox = new QGroupBox(tr("Image"));

    scale_spin = new QSpinBox;
    scale_spin->setSingleStep(25);
    scale_spin->setMinimum(1);
    scale_spin->setMaximum(5000);
    scale_spin->setSuffix(" %");
    scale_spin->setValue(100);

    scale_slider = new QSlider(Qt::Horizontal, this);
    scale_slider->setTickPosition(QSlider::TicksAbove);
    scale_slider->setMinimum(1);
    scale_slider->setMaximum(1000);
    scale_slider->setTickInterval(100);
    scale_slider->setSingleStep(25);
    scale_slider->setValue(100);

    connect(scale_spin,SIGNAL(valueChanged(int)),this,SLOT(do_scale(int)));
    connect(scale_slider,SIGNAL(valueChanged(int)),scale_spin,SLOT(setValue(int)));
    connect(scale_spin,SIGNAL(valueChanged(int)),scale_slider,SLOT(setValue(int)));
    connect(scale_spin0,SIGNAL(valueChanged(int)),scale_spin,SLOT(setValue(int)));

    scale_spin->installEventFilter(this);
    scale_slider->installEventFilter(this);
    scale_spin->setMouseTracking(true);
    scale_slider->setMouseTracking(true);

    histo_checkbox = new QCheckBox(tr("histogram normalization for the gradient"));
    connect(histo_checkbox,SIGNAL(clicked()),this,SLOT(filtering_visu()));

    QFormLayout* size_layout = new QFormLayout;
    size_layout->addRow(tr("scale ="), scale_spin);
    size_layout->addRow(scale_slider);
    size_layout->addRow(" ",histo_checkbox);
    if( operating_system == 2 )
    {
        size_layout->setFormAlignment(Qt::AlignLeft);
    }

    size_groupbox->setLayout(size_layout);

    ////////////////////////////////////////////////////////////////////////////////////////////////

    step_checkbox = new QCheckBox(tr("display"));
    step_checkbox->setToolTip(tr("to see the calculation time if not checked"));

    QGroupBox* active_contour_groupbox = new QGroupBox(tr("Active contour"));

    outsidecolor_combobox = new QComboBox();
    insidecolor_combobox = new QComboBox();

    // QPixmap pm : petite image affichant la couleur devant le nom de la couleur dans le combobox
    QPixmap pm(12,12);
    pm.fill(Qt::red);
    outsidecolor_combobox->addItem (pm, tr("Red"));
    insidecolor_combobox->addItem (pm, tr("Red"));
    pm.fill(Qt::green);
    outsidecolor_combobox->addItem (pm, tr("Green"));
    insidecolor_combobox->addItem (pm, tr("Green"));
    pm.fill(Qt::blue);
    outsidecolor_combobox->addItem (pm, tr("Blue"));
    insidecolor_combobox->addItem (pm, tr("Blue"));
    pm.fill(Qt::cyan);
    outsidecolor_combobox->addItem (pm, tr("Cyan"));
    insidecolor_combobox->addItem (pm, tr("Cyan"));
    pm.fill(Qt::magenta);
    outsidecolor_combobox->addItem (pm, tr("Magenta"));
    insidecolor_combobox->addItem (pm, tr("Magenta"));
    pm.fill(Qt::yellow);
    outsidecolor_combobox->addItem (pm, tr("Yellow"));
    insidecolor_combobox->addItem (pm, tr("Yellow"));
    pm.fill(Qt::black);
    outsidecolor_combobox->addItem (pm, tr("Black"));
    insidecolor_combobox->addItem (pm, tr("Black"));
    pm.fill(Qt::white);
    outsidecolor_combobox->addItem (pm, tr("White"));
    insidecolor_combobox->addItem (pm, tr("White"));
    pm.fill(Qt::transparent);
    outsidecolor_combobox->addItem (pm,tr("Selected"));
    insidecolor_combobox->addItem (pm,tr("Selected"));
    outsidecolor_combobox->addItem (pm, tr("No"));
    insidecolor_combobox->addItem (pm, tr("No"));


    connect(outsidecolor_combobox,SIGNAL(activated(int)),this,SLOT(phi_visu(int)));
    connect(insidecolor_combobox,SIGNAL(activated(int)),this,SLOT(phi_visu(int)));

    QPushButton* outsidecolor_select = new QPushButton(tr("Select"));
    connect(outsidecolor_select,SIGNAL(clicked()),this,SLOT(color_out()));

    QPushButton* insidecolor_select = new QPushButton(tr("Select"));
    connect(insidecolor_select,SIGNAL(clicked()),this,SLOT(color_in()));

    init_time_spin = new QDoubleSpinBox;
    init_time_spin->setSingleStep(0.5);
    init_time_spin->setMinimum(0.0);
    init_time_spin->setMaximum(1000.0);
    init_time_spin->setSuffix(tr(" second(s)"));
    init_time_spin->setToolTip(tr("time to see the active contour initialization"));

    QFormLayout* active_contour_form = new QFormLayout;
    active_contour_form->addRow(tr("after each iteration :"), step_checkbox);
    active_contour_form->addRow(tr("time before to evolve ="), init_time_spin);
    if( operating_system == 2 )
    {
        active_contour_form->setFormAlignment(Qt::AlignLeft);
    }

    QFormLayout* Loutcolor_form = new QFormLayout;
    Loutcolor_form->addRow(tr("Lout :"), outsidecolor_combobox);
    if( operating_system == 2 )
    {
        Loutcolor_form->setFormAlignment(Qt::AlignLeft);
    }

    QFormLayout* Lincolor_form = new QFormLayout;
    Lincolor_form->addRow(tr("  Lin :"), insidecolor_combobox);
    if( operating_system == 2 )
    {
        Lincolor_form->setFormAlignment(Qt::AlignLeft);
    }

    QHBoxLayout* Loutcolor_hlay = new QHBoxLayout;
    Loutcolor_hlay->addLayout(Loutcolor_form);
    Loutcolor_hlay->addWidget(outsidecolor_select);

    QHBoxLayout* Lincolor_hlay = new QHBoxLayout;
    Lincolor_hlay->addLayout(Lincolor_form);
    Lincolor_hlay->addWidget(insidecolor_select);

    QVBoxLayout* color_layout = new QVBoxLayout;
    color_layout->addLayout(Loutcolor_hlay);
    color_layout->addLayout(Lincolor_hlay);

    QGroupBox* color_groupbox = new QGroupBox(tr("Boundaries colors"));
    color_groupbox->setFlat(true);
    color_groupbox->setLayout(color_layout);

    QVBoxLayout* active_contour_layout = new QVBoxLayout;
    active_contour_layout->addLayout(active_contour_form);
    active_contour_layout->addWidget(color_groupbox);

    active_contour_groupbox->setLayout(active_contour_layout);

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////

    QVBoxLayout* display_layout = new QVBoxLayout;
    display_layout->addWidget(size_groupbox);
    display_layout->addWidget(active_contour_groupbox);
    display_layout->addStretch(1);

    return display_layout;
}

///// Fin de la création de la fenêtre de configuration ///////////


// Fonction appelée lors de l'ouverture de la fenêtre de configuration et valider ou refuser les changements
void ImageViewer::settings()
{
    if( onglets->currentIndex() == 1 )
    {
        filtering_visu();
        phi_visu(true);
        shape_visu();
    }
    else
    {
        filtering_visu();
        phi_visu(false);
    }

    // on s'assure que la fenêtre de configuration n'est pas déja affichée
    if( settings_window->isVisible() )
    {
        QMessageBox::critical(this, tr("Error"), tr("Close settings window before."));
    }
    else
    {
        // booléen permettant de savoir si l'utilisateur accepte ou rejette les réglages de la fenêtre de configuration
        int result_settings_window;

        // la méthode exec() permet d'afficher la fenêtre de configuration settings_window de maniere modale,
        // de plus cette méthode retourne un booléen en fonction de l'acceptation ou du rejet des réglages
        result_settings_window = settings_window->exec();

        // la méthode show() permet d'afficher la fenêtre de configuration mais de maniere non modale,
        //  ne renvoie pas de booléen
        //settings_window->show();

        if( result_settings_window )
        {
            // Ok = les variables vont prendre leur valeur en fonction de l'état des widgets de la fenêtre de configuration
            apply_settings();

        }
        else
        {
            // Annuler = les widgets vont reprendre leur état en fonction des valeurs des variables
            cancel_settings();
        }
    }
}

// si on clique sur le boutton Ok de settings_window
// les paramètres prennent leurs valeurs en fonction de l'état des widgets
void ImageViewer::apply_settings()
{
    // affection des variables en fonction des états des widgets

    ///////////////////////////////////
    //          Algorithm            //
    ///////////////////////////////////

    Na1 = Na_spin->value();

    if( chanvese_radio->isChecked() )
    {
        model = 1;
    }
    if( geodesic_radio->isChecked() )
    {
        model = 2;
    }

    lambda_out1 = lambda_out_spin->value();
    lambda_in1 = lambda_in_spin->value();

    kernel_gradient_length1 = klength_gradient_spin->value();

    alpha1 = Yspin->value();
    beta1 = Uspin->value();
    gamma1 = Vspin->value();

    if( internalspeed_groupbox->isChecked() )
    {
        hasSmoothingCycle1 = true;
    }
    else
    {
        hasSmoothingCycle1 = false;
    }

    Ns1 = Ns_spin->value();

    kernel_curve1 = klength_spin->value();
    std_curve1 = std_spin->value();


    ///////////////////////////////////
    //       Initialization          //
    ///////////////////////////////////

    if( rectangle_radio->isChecked() )
    {
        hasEllipse1 = false;
    }
    if( ellipse_radio->isChecked() )
    {
        hasEllipse1 = true;
    }

    init_width1 = double(width_shape_spin->value())/100.0;
    init_height1 = double(height_shape_spin->value())/100.0;

    center_x1 = double(abscissa_spin->value())/100.0;
    center_y1 = double(ordinate_spin->value())/100.0;

    ///////////////////////////////////
    //        Preprocessing          //
    ///////////////////////////////////

    if( preprocessing_groupbox->isChecked() )
    {
        hasPreprocess = true;
    }
    else
    {
        hasPreprocess = false;
    }

    if( gaussian_noise_groupbox->isChecked() )
    {
        hasGaussianNoise = true;
    }
    else
    {
        hasGaussianNoise = false;
    }
    std_noise = std_noise_spin->value();

    if( salt_noise_groupbox->isChecked() )
    {
        hasSaltNoise = true;
    }
    else
    {
        hasSaltNoise = false;
    }
    proba_noise = proba_noise_spin->value()/100.0;

    if( speckle_noise_groupbox->isChecked() )
    {
        hasSpeckleNoise = true;
    }
    else
    {
        hasSpeckleNoise = false;
    }
    std_speckle_noise = std_speckle_noise_spin->value();

    if( median_groupbox->isChecked() )
    {
        hasMedianFilt = true;
    }
    else
    {
        hasMedianFilt = false;
    }
    kernel_median_length1 = klength_median_spin->value();
    if( complex_radio2->isChecked() )
    {
        hasO1algo1 = true;
    }
    else
    {
        hasO1algo1 = false;
    }

    if( mean_groupbox->isChecked() )
    {
        hasMeanFilt = true;
    }
    else
    {
        hasMeanFilt = false;
    }
    kernel_mean_length1 = klength_mean_spin->value();

    if( gaussian_groupbox->isChecked() )
    {
        hasGaussianFilt = true;
    }
    else
    {
        hasGaussianFilt = false;
    }
    kernel_gaussian_length1 = klength_gaussian_spin->value();
    sigma = std_filter_spin->value();

    if( aniso_groupbox->isChecked() )
    {
        hasAnisoDiff = true;
    }
    else
    {
        hasAnisoDiff = false;
    }

    max_itera1 = iteration_filter_spin->value();
    lambda1 = lambda_spin->value();
    kappa1 = kappa_spin->value();
    if( aniso1_radio->isChecked() )
    {
        aniso_option1 = 1;
    }
    if( aniso2_radio->isChecked() )
    {
        aniso_option1 = 2;
    }

    if( open_groupbox->isChecked() )
    {
        hasOpenFilt = true;
    }
    else
    {
        hasOpenFilt = false;
    }
    kernel_open_length1 = klength_open_spin->value();

    if( close_groupbox->isChecked() )
    {
        hasCloseFilt = true;
    }
    else
    {
        hasCloseFilt = false;
    }
    kernel_close_length1 = klength_close_spin->value();

    if( tophat_groupbox->isChecked() )
    {
        hasTophatFilt = true;
    }
    else
    {
        hasTophatFilt = false;
    }
    if( whitetophat_radio->isChecked() )
    {
        isWhiteTophat = true;
    }
    else
    {
        isWhiteTophat = false;
    }
    kernel_tophat_length1 = klength_tophat_spin->value();

    if( complex2_morpho_radio->isChecked() )
    {
        hasO1morpho1 = true;
    }
    else
    {
        hasO1morpho1 = false;
    }

    /////////////////////////////
    //        Display          //
    /////////////////////////////

    scale_spin0->setValue(scale_spin->value());

    if( histo_checkbox->isChecked() )
    {
        hasHistoNormaliz = true;
    }
    else
    {
        hasHistoNormaliz = false;
    }



    if( step_checkbox->isChecked() )
    {
        hasDisplayEach = true;
    }
    else
    {
        hasDisplayEach = false;
    }

    seconds_wait = init_time_spin->value();


    inside_combo = insidecolor_combobox->currentIndex();
    outside_combo = outsidecolor_combobox->currentIndex();

    Rin_selected1 = Rin_selected2;
    Gin_selected1 = Gin_selected2;
    Bin_selected1 = Bin_selected2;

    Rout_selected1 = Rout_selected2;
    Gout_selected1 = Gout_selected2;
    Bout_selected1 = Bout_selected2;



    ////////////////////////////////////////////////////////

    if( (phi_init1 != NULL) && (phi_init2 != NULL) )
    {
        std::memcpy(phi_init1,phi_init2,img_size);
    }
    clean_boundaries(phi_init2,phi_init2_clean);
    if( (phi_init1_clean != NULL) && (phi_init2_clean != NULL) )
    {
        std::memcpy(phi_init1_clean,phi_init2_clean,img_size);
    }
    phiInit2imgPhi();

    QSettings settings("Bessy", "Ofeli");

    settings.setValue("Settings/Algorithm/Na", Na1);
    settings.setValue("Settings/Algorithm/Ns", Ns1);
    settings.setValue("Settings/Algorithm/model", model);
    settings.setValue("Settings/Algorithm/isRgb", isRgb1);
    settings.setValue("Settings/Algorithm/lambda_out", lambda_out1);
    settings.setValue("Settings/Algorithm/lambda_in", lambda_in1);
    settings.setValue("Settings/Algorithm/kernel_gradient_length", kernel_gradient_length1);
    settings.setValue("Settings/Algorithm/alpha", alpha1);
    settings.setValue("Settings/Algorithm/beta", beta1);
    settings.setValue("Settings/Algorithm/gamma", gamma1);
    settings.setValue("Settings/Algorithm/hasSmoothingCycle", hasSmoothingCycle1);
    settings.setValue("Settings/Algorithm/kernel_curve", kernel_curve1);
    settings.setValue("Settings/Algorithm/std_curve", std_curve1);

    //settings.setValue("Settings/Initialization/img_phi", img_phi);
    settings.setValue("Settings/Initialization/hasEllipse", hasEllipse1);
    settings.setValue("Settings/Initialization/init_width", init_width1);
    settings.setValue("Settings/Initialization/init_height", init_height1);
    settings.setValue("Settings/Initialization/center_x", center_x1);
    settings.setValue("Settings/Initialization/center_y", center_y1);

    settings.setValue("Settings/Preprocessing/hasPreprocess", hasPreprocess);
    settings.setValue("Settings/Preprocessing/hasGaussianNoise", hasGaussianNoise);
    settings.setValue("Settings/Preprocessing/std_noise", std_noise);
    settings.setValue("Settings/Preprocessing/hasSaltNoise", hasSaltNoise);
    settings.setValue("Settings/Preprocessing/proba_noise", proba_noise);
    settings.setValue("Settings/Preprocessing/hasSpeckleNoise", hasSpeckleNoise);
    settings.setValue("Settings/Preprocessing/std_speckle_noise", std_speckle_noise);

    settings.setValue("Settings/Preprocessing/hasMedianFilt", hasMedianFilt);
    settings.setValue("Settings/Preprocessing/kernel_median_length", kernel_median_length1);
    settings.setValue("Settings/Preprocessing/hasO1algo", hasO1algo1);
    settings.setValue("Settings/Preprocessing/hasMeanFilt", hasMeanFilt);
    settings.setValue("Settings/Preprocessing/kernel_mean_length", kernel_mean_length1);
    settings.setValue("Settings/Preprocessing/hasGaussianFilt", hasGaussianFilt);
    settings.setValue("Settings/Preprocessing/kernel_gaussian_length", kernel_gaussian_length1);
    settings.setValue("Settings/Preprocessing/sigma", sigma);

    settings.setValue("Settings/Preprocessing/hasAnisoDiff", hasAnisoDiff);
    settings.setValue("Settings/Preprocessing/aniso_option", aniso_option1);
    settings.setValue("Settings/Preprocessing/max_itera", max_itera1);
    settings.setValue("Settings/Preprocessing/lambda", lambda1);
    settings.setValue("Settings/Preprocessing/kappa", kappa1);

    settings.setValue("Settings/Preprocessing/hasOpenFilt", hasOpenFilt);
    settings.setValue("Settings/Preprocessing/kernel_open_length", kernel_open_length1);

    settings.setValue("Settings/Preprocessing/hasCloseFilt", hasCloseFilt);
    settings.setValue("Settings/Preprocessing/kernel_close_length", kernel_close_length1);

    settings.setValue("Settings/Preprocessing/hasTophatFilt", hasTophatFilt);
    settings.setValue("Settings/Preprocessing/isWhiteTophat", isWhiteTophat);
    settings.setValue("Settings/Preprocessing/kernel_tophat_length", kernel_tophat_length1);

    settings.setValue("Settings/Preprocessing/hasO1morpho", hasO1morpho1);


    settings.setValue("Settings/Display/hasHistoNormaliz", hasHistoNormaliz);

    settings.setValue("Settings/Display/hasDisplayEach", hasDisplayEach);

    settings.setValue("Settings/Display/inside_combo", inside_combo);
    settings.setValue("Settings/Display/outside_combo", outside_combo);

    settings.setValue("Settings/Display/Rout_selected", int(Rout_selected1));
    settings.setValue("Settings/Display/Gout_selected", int(Gout_selected1));
    settings.setValue("Settings/Display/Bout_selected", int(Bout_selected1));
    settings.setValue("Settings/Display/Rin_selected", int(Rin_selected1));
    settings.setValue("Settings/Display/Gin_selected", int(Gin_selected1));
    settings.setValue("Settings/Display/Bin_selected", int(Bin_selected1));

    settings.setValue("Settings/Display/seconds_wait", seconds_wait);
}

// si on clique sur le boutton Cancel de settings_window
// les widgets reprennent leur état, correspondant aux valeurs des paramètres
void  ImageViewer::cancel_settings()
{
    ///////////////////////////////////
    //          Algorithm            //
    ///////////////////////////////////

    Na_spin->setValue(Na1);

    if( model == 1 )
    {
        chanvese_radio->setChecked(true);
    }
    if( model == 2 )
    {
        geodesic_radio->setChecked(true);
    }

    lambda_out_spin->setValue(lambda_out1);
    lambda_in_spin->setValue(lambda_in1);

    klength_gradient_spin->setValue(kernel_gradient_length1);

    Yspin->setValue(alpha1);
    Uspin->setValue(beta1);
    Vspin->setValue(gamma1);

    Ns_spin->setValue(Ns1);

    if( hasSmoothingCycle1 )
    {
        internalspeed_groupbox->setChecked(true);
    }
    else
    {
        internalspeed_groupbox->setChecked(false);
    }

    klength_spin->setValue(kernel_curve1);
    std_spin->setValue(std_curve1);

    ///////////////////////////////////
    //       Initialization          //
    ///////////////////////////////////

    if( !hasEllipse1 )
    {
        rectangle_radio->setChecked(true);
    }
    else
    {
        ellipse_radio->setChecked(true);
    }

    width_shape_spin->setValue(int(init_width1*100.0));
    height_shape_spin->setValue(int(init_height1*100.0));

    abscissa_spin->setValue(int(center_x1*100.0));
    ordinate_spin->setValue(int(center_y1*100.0));

    ///////////////////////////////////
    //        Preprocessing          //
    ///////////////////////////////////

    if( hasPreprocess )
    {
        preprocessing_groupbox->setChecked(true);
    }
    else
    {
       preprocessing_groupbox->setChecked(false);
    }

    if( hasGaussianNoise )
    {
        gaussian_noise_groupbox->setChecked(true);
    }
    else
    {
        gaussian_noise_groupbox->setChecked(false);
    }


    std_noise_spin->setValue(std_noise);

    if( hasSaltNoise )
    {
        salt_noise_groupbox->setChecked(true);
    }
    else
    {
        salt_noise_groupbox->setChecked(false);
    }

    proba_noise_spin->setValue(100.0*proba_noise);

    if( hasSpeckleNoise )
    {
        speckle_noise_groupbox->setChecked(true);
    }
    else
    {
        speckle_noise_groupbox->setChecked(false);
    }

    std_speckle_noise_spin->setValue(std_speckle_noise);

    if( hasMedianFilt )
    {
        median_groupbox->setChecked(true);
    }
    else
    {
        median_groupbox->setChecked(false);
    }

    klength_median_spin->setValue(kernel_median_length1);

    if( hasO1algo1 )
    {
        complex_radio2->setChecked(true);
    }
    else
    {
        complex_radio1->setChecked(true);
    }

    if( hasMeanFilt )
    {
        mean_groupbox->setChecked(true);
    }
    else
    {
        mean_groupbox->setChecked(false);
    }
    klength_mean_spin->setValue(kernel_mean_length1);

    if( hasGaussianFilt )
    {
        gaussian_groupbox->setChecked(true);
    }
    else
    {
        gaussian_groupbox->setChecked(false);
    }
    klength_gaussian_spin->setValue(kernel_gaussian_length1);
    std_filter_spin->setValue(sigma);

    if( hasAnisoDiff )
    {
        aniso_groupbox->setChecked(true);
    }
    else
    {
        aniso_groupbox->setChecked(false);
    }

    iteration_filter_spin->setValue(max_itera1);
    lambda_spin->setValue(lambda1);
    kappa_spin->setValue(kappa1);
    if( aniso_option1 == 1 )
    {
        aniso1_radio->setChecked(true);
    }
    if( aniso_option1 == 2 )
    {
        aniso2_radio->setChecked(true);
    }

    if( hasOpenFilt )
    {
        open_groupbox->setChecked(true);
    }
    else
    {
        open_groupbox->setChecked(false);
    }
    klength_open_spin->setValue(kernel_open_length1);

    if( hasCloseFilt )
    {
        close_groupbox->setChecked(true);
    }
    else
    {
        close_groupbox->setChecked(false);
    }
    klength_close_spin->setValue(kernel_close_length1);

    if( hasTophatFilt )
    {
        tophat_groupbox->setChecked(true);
    }
    else
    {
        tophat_groupbox->setChecked(false);
    }
    if( isWhiteTophat )
    {
        whitetophat_radio->setChecked(true);
    }
    else
    {
        blacktophat_radio->setChecked(true);
    }
    klength_tophat_spin->setValue(kernel_tophat_length1);

    if( hasO1morpho1 )
    {
        complex2_morpho_radio->setChecked(true);
    }
    else
    {
        complex1_morpho_radio->setChecked(true);
    }

    /////////////////////////////
    //        Display          //
    /////////////////////////////

    scale_spin->setValue(scale_spin0->value());

    if( hasHistoNormaliz )
    {
        histo_checkbox->setChecked(true);
    }
    else
    {
        histo_checkbox->setChecked(false);
    }

    if( hasDisplayEach )
    {
        step_checkbox->setChecked(true);
    }
    else
    {
        step_checkbox->setChecked(false);
    }

    init_time_spin->setValue(seconds_wait);


    insidecolor_combobox->setCurrentIndex(inside_combo);
    outsidecolor_combobox->setCurrentIndex(outside_combo);

    Rin_selected2 = Rin_selected1;
    Gin_selected2 = Gin_selected1;
    Bin_selected2 = Bin_selected1;

    Rout_selected2 = Rout_selected1;
    Gout_selected2 = Gout_selected1;
    Bout_selected2 = Bout_selected1;

    unsigned char R, G, B;

    if( inside_combo == 8 )
    {
        R = Rin_selected2;
        G = Gin_selected2;
        B = Bin_selected2;
    }
    else
    {
        get_color(inside_combo,R,G,B);
    }
    QPixmap pm1(12,12);
    pm1.fill(QColor(R,G,B));
    insidecolor_combobox->setItemIcon(8,pm1);

    if( outside_combo == 8 )
    {
        R = Rout_selected2;
        G = Gout_selected2;
        B = Bout_selected2;
    }
    else
    {
        get_color(outside_combo,R,G,B);
    }
    QPixmap pm2(12,12);
    pm2.fill(QColor(R,G,B));
    outsidecolor_combobox->setItemIcon(8,pm2);

    ////////////////////////////////////////////////////////

    if( (phi_init1 != NULL) && (phi_init2 != NULL) )
    {
        std::memcpy(phi_init2,phi_init1,img_size);
    }
    clean_boundaries(phi_init1,phi_init1_clean);
    if( (phi_init1_clean != NULL) && (phi_init2_clean != NULL) )
    {
        std::memcpy(phi_init2_clean,phi_init1_clean,img_size);
    }

    phi_visu(true);
    shape_visu();

    phiInit2imgPhi();

    QSettings settings("Bessy", "Ofeli");

    settings.setValue("Settings/Algorithm/Na", Na1);
    settings.setValue("Settings/Algorithm/Ns", Ns1);
    settings.setValue("Settings/Algorithm/model", model);
    settings.setValue("Settings/Algorithm/isRgb", isRgb1);
    settings.setValue("Settings/Algorithm/lambda_out", lambda_out1);
    settings.setValue("Settings/Algorithm/lambda_in", lambda_in1);
    settings.setValue("Settings/Algorithm/kernel_gradient_length", kernel_gradient_length1);
    settings.setValue("Settings/Algorithm/alpha", alpha1);
    settings.setValue("Settings/Algorithm/beta", beta1);
    settings.setValue("Settings/Algorithm/gamma", gamma1);
    settings.setValue("Settings/Algorithm/hasSmoothingCycle", hasSmoothingCycle1);
    settings.setValue("Settings/Algorithm/kernel_curve", kernel_curve1);
    settings.setValue("Settings/Algorithm/std_curve", std_curve1);

    //settings.setValue("Settings/Initialization/img_phi", img_phi);
    settings.setValue("Settings/Initialization/hasEllipse", hasEllipse1);
    settings.setValue("Settings/Initialization/init_width", init_width1);
    settings.setValue("Settings/Initialization/init_height", init_height1);
    settings.setValue("Settings/Initialization/center_x", center_x1);
    settings.setValue("Settings/Initialization/center_y", center_y1);

    settings.setValue("Settings/Preprocessing/hasPreprocess", hasPreprocess);
    settings.setValue("Settings/Preprocessing/hasGaussianNoise", hasGaussianNoise);
    settings.setValue("Settings/Preprocessing/std_noise", std_noise);
    settings.setValue("Settings/Preprocessing/hasSaltNoise", hasSaltNoise);
    settings.setValue("Settings/Preprocessing/proba_noise", proba_noise);
    settings.setValue("Settings/Preprocessing/hasSpeckleNoise", hasSpeckleNoise);
    settings.setValue("Settings/Preprocessing/std_speckle_noise", std_speckle_noise);

    settings.setValue("Settings/Preprocessing/hasMedianFilt", hasMedianFilt);
    settings.setValue("Settings/Preprocessing/kernel_median_length", kernel_median_length1);
    settings.setValue("Settings/Preprocessing/hasO1algo", hasO1algo1);
    settings.setValue("Settings/Preprocessing/hasMeanFilt", hasMeanFilt);
    settings.setValue("Settings/Preprocessing/kernel_mean_length", kernel_mean_length1);
    settings.setValue("Settings/Preprocessing/hasGaussianFilt", hasGaussianFilt);
    settings.setValue("Settings/Preprocessing/kernel_gaussian_length", kernel_gaussian_length1);
    settings.setValue("Settings/Preprocessing/sigma", sigma);

    settings.setValue("Settings/Preprocessing/hasAnisoDiff", hasAnisoDiff);
    settings.setValue("Settings/Preprocessing/aniso_option", aniso_option1);
    settings.setValue("Settings/Preprocessing/max_itera", max_itera1);
    settings.setValue("Settings/Preprocessing/lambda", lambda1);
    settings.setValue("Settings/Preprocessing/kappa", kappa1);

    settings.setValue("Settings/Preprocessing/hasOpenFilt", hasOpenFilt);
    settings.setValue("Settings/Preprocessing/kernel_open_length", kernel_open_length1);

    settings.setValue("Settings/Preprocessing/hasCloseFilt", hasCloseFilt);
    settings.setValue("Settings/Preprocessing/kernel_close_length", kernel_close_length1);

    settings.setValue("Settings/Preprocessing/hasTophatFilt", hasTophatFilt);
    settings.setValue("Settings/Preprocessing/isWhiteTophat", isWhiteTophat);
    settings.setValue("Settings/Preprocessing/kernel_tophat_length", kernel_tophat_length1);

    settings.setValue("Settings/Preprocessing/hasO1morpho", hasO1morpho1);


    settings.setValue("Settings/Display/hasHistoNormaliz", hasHistoNormaliz);

    settings.setValue("Settings/Display/hasDisplayEach", hasDisplayEach);

    settings.setValue("Settings/Display/inside_combo", inside_combo);
    settings.setValue("Settings/Display/outside_combo", outside_combo);

    settings.setValue("Settings/Display/Rout_selected", int(Rout_selected1));
    settings.setValue("Settings/Display/Gout_selected", int(Gout_selected1));
    settings.setValue("Settings/Display/Bout_selected", int(Bout_selected1));
    settings.setValue("Settings/Display/Rin_selected", int(Rin_selected1));
    settings.setValue("Settings/Display/Gin_selected", int(Gin_selected1));
    settings.setValue("Settings/Display/Bin_selected", int(Bin_selected1));

    settings.setValue("Settings/Display/seconds_wait", seconds_wait);
}

void ImageViewer::default_settings()
{
    ///////////////////////////////////////////////////

    // paramètres par défaut de l'onglet Algorithm

    ////////////////////////////////////////////////////

    Na1 = 30;
    Ns1 = 3;
    model = 1;
    lambda_out1 = 1;
    lambda_in1 = 1;
    kernel_gradient_length1 = 5;
    alpha1 = 1;
    beta1 = 10;
    gamma1 = 10;
    hasSmoothingCycle1 = true;
    kernel_curve1 = 5;
    std_curve1 = 2.0;

    //////////////////////////////////////////////////

    ///////////////////////////////////////////////////

    // paramètres par défaut de l'onglet Initialisation

    ////////////////////////////////////////////////////

    hasEllipse1 = false;
    init_width1 = 0.95;
    init_height1 = 0.95;
    center_x1 = 0.0;
    center_y1 = 0.0;

    ////////////////////////////////////////////////////

    ///////////////////////////////////////////////////

    // paramètres par défaut de l'onglet Preprocessing

    ////////////////////////////////////////////////////

    hasPreprocess = false;
    hasGaussianNoise = false;
    std_noise = 20.0;
    hasSaltNoise = false;
    proba_noise = 0.05;
    hasSpeckleNoise = false;
    std_speckle_noise = 0.16;

    hasMedianFilt = false;
    kernel_median_length1 = 5;
    hasO1algo1 = true;

    hasMeanFilt = false;
    kernel_mean_length1 = 5;

    hasGaussianFilt = false;
    kernel_gaussian_length1 = 5;
    sigma = 2.0;

    hasAnisoDiff = false;
    aniso_option1 = 1;
    max_itera1 = 10;
    lambda1 = 1.0/7.0;
    kappa1 = 30.0;

    hasOpenFilt = false;
    kernel_open_length1 = 5;

    hasCloseFilt = false;
    kernel_close_length1 = 5;

    hasTophatFilt = false;
    isWhiteTophat = true;
    kernel_tophat_length1 = 5;

    hasO1morpho1 = true;

    ////////////////////////////////////////////

    // paramètres par défaut de l'onglet Display

    ///////////////////////////////////////////

    hasHistoNormaliz = true;

    hasDisplayEach = true;
    inside_combo = 0;
    outside_combo = 2;

    Rout_selected1 = 128;
    Gout_selected1 = 0;
    Bout_selected1 = 255;

    Rin_selected1 = 255;
    Gin_selected1 = 128;
    Bin_selected1 = 0;

    seconds_wait = 0.0;

    hasEllipse1 = true;
    init_width1 = 0.65;
    init_height1 = 0.65;
    center_x1 = 0.0;
    center_y1 = 0.0;
    cancel_settings();

    scale_spin->setValue(100);

    QImage img_phi0 = QImage(100,100, QImage::Format_Indexed8);

    int x, y; // position

    for( int offset = 0; offset < 10000; offset++ )
    {
        // offset = x+y*img_width <==>
        y = offset/100;
        x = offset-y*100;

        if( x < 95 && x > 5 && y < 95 && y > 5 )
        {
            *( img_phi0.scanLine(y)+x ) = 255;
        }
        else
        {
            *( img_phi0.scanLine(y)+x ) = 0;
        }
    }

    img_phi = img_phi0.scaled( img_width, img_height, Qt::IgnoreAspectRatio, Qt::FastTransformation);

    imgPhi2phiInit();

    // pour afficher l'image dans la fenetre settings
    int n_out = 0;
    int n_in = 0;
    for( int offset = 0; offset < img_size; offset++ )
    {
        if( phi_init1_clean[offset] == 1 )
        {
            Lout_2[n_out++] = offset;
        }
        if( phi_init1_clean[offset] == -1 )
        {
            Lin_2[n_in++] = offset;
        }
    }
    Lout_2[n_out] = list_end;
    Lin_2[n_in] = list_end;

    filtering_visu();
    tab_visu(onglets->currentIndex());
}

// Fonction appelée pour le changement d 'echelle de l'image dans la fenêtre paramètre
void ImageViewer::do_scale(int value)
{
    if( img1 != NULL )
    {
        double scale_factor = value/100.0;
        imageLabel_settings->setZoomFactor(scale_factor);

    }
}

// Nettoyage des frontières
void ImageViewer::clean_boundaries(char* phi, char* phi_clean)
{
    if( phi != NULL && phi_clean != NULL )
    {
        int offset, x, y; // position

        for( offset = 0; offset < img_size; offset++ )
        {
            if( phi[offset] == 1 )
            {
                // offset = x+y*img_width <==>
                y = offset/img_width;
                x = offset-y*img_width;

                // nettoyage si la condition de voisinage aux frontières n'est pas respectée
                if( isRedundantPointOfLout(phi,x,y) )
                {
                    phi_clean[offset] = 3;
                }
                else
                {
                    phi_clean[offset] = 1;
                }
            }

            if( phi[offset] == -1 )
            {
                // offset = x+y*img_width <==>
                y = offset/img_width;
                x = offset-y*img_width;

                if( isRedundantPointOfLin(phi,x,y) )
                {
                    phi_clean[offset] = -3;
                }
                else
                {
                    phi_clean[offset] = -1;
                }
            }
        }

        // efface l'affichage des listes
        for( int n_out = 0; Lout_2[n_out] != list_end; n_out++ )
        {
            offset = Lout_2[n_out]*3;

            image_phi_uchar[offset] = image_filter_uchar[offset];
            image_phi_uchar[offset+1] = image_filter_uchar[offset+1];
            image_phi_uchar[offset+2] = image_filter_uchar[offset+2];

            image_shape_uchar[offset] = image_filter_uchar[offset];
            image_shape_uchar[offset+1] = image_filter_uchar[offset+1];
            image_shape_uchar[offset+2] = image_filter_uchar[offset+2];
        }

        for( int n_in = 0; Lin_2[n_in] != list_end; n_in++ )
        {
            offset = Lin_2[n_in]*3;

            image_phi_uchar[offset] = image_filter_uchar[offset];
            image_phi_uchar[offset+1] = image_filter_uchar[offset+1];
            image_phi_uchar[offset+2] = image_filter_uchar[offset+2];

            image_shape_uchar[offset] = image_filter_uchar[offset];
            image_shape_uchar[offset+1] = image_filter_uchar[offset+1];
            image_shape_uchar[offset+2] = image_filter_uchar[offset+2];
        }

        int n_out = 0;
        int n_in = 0;
        for( int offset = 0; offset < img_size; offset++ )
        {
            if( phi_clean[offset] == 1 )
            {
                Lout_2[n_out++] = offset;
            }
            if( phi_clean[offset] == -1 )
            {
                Lin_2[n_in++] = offset;
            }
        }
        Lout_2[n_out] = list_end;
        Lin_2[n_in] = list_end;

    }
    return;
}

bool ImageViewer::isRedundantPointOfLin(char* phi, int x, int y)
{
    // if ∃ a neighbor ∈ Lout | ∈ Rout

    if( y-1 >= 0 )
    {
        if( phi[ x+(y-1)*img_width ] >= 0 )
        {
            return false; // is not redundant point of Lin
        }
    }
    if( x-1 >= 0 )
    {
        if( phi[ (x-1)+y*img_width ] >= 0 )
        {
            return false; // is not redundant point of Lin
        }
    }
    if( x+1 < img_width )
    {
        if( phi[ (x+1)+y*img_width ] >= 0 )
        {
            return false; // is not redundant point of Lin
        }
    }
    if( y+1 < img_height )
    {
        if( phi[ x+(y+1)*img_width ] >= 0 )
        {
            return false; // is not redundant point of Lin
        }
    }

    // ==> ∀ neighbors ∈ Lin | ∈ Rin
    return true; // is redundant point of Lin
}

bool ImageViewer::isRedundantPointOfLout(char* phi, int x, int y)
{
    // if ∃ a neighbor ∈ Lin | ∈ Rin

    if( y-1 >= 0 )
    {
        if( phi[ x+(y-1)*img_width ] <= 0 )
        {
            return false; // is not redundant point of Lout
        }
    }
    if( x-1 >= 0 )
    {
        if( phi[ (x-1)+y*img_width ] <= 0 )
        {
            return false; // is not redundant point of Lout
        }
    }
    if( x+1 < img_width )
    {
        if( phi[ (x+1)+y*img_width ] <= 0 )
        {
            return false; // is not redundant point of Lout
        }
    }
    if( y+1 < img_height )
    {
        if( phi[ x+(y+1)*img_width ] <= 0 )
        {
            return false; // is not redundant point of Lout
        }
    }

    // ==> ∀ neighbors ∈ Lout | ∈ Rout
    return true; // is redundant point of Lout
}

// Fonction appelée dans l'onglet initalisation pour calculer et afficher l'image+phi(couleur foncé)+forme(couleur clair)
void ImageViewer::shape_visu()
{
    if( shape != NULL )
    {
        init_width2 = double(width_shape_spin->value())/100.0;
        init_height2 = double(height_shape_spin->value())/100.0;
        center_x2 = double(abscissa_spin->value())/100.0;
        center_y2 = double(ordinate_spin->value())/100.0;

        if( rectangle_radio->isChecked() )
        {
            hasEllipse2 = false;
        }
        if( ellipse_radio->isChecked() )
        {
            hasEllipse2 = true;
        }


        int offset, x, y; // position

        // efface les listes de la QImage
        for( int n_out = 0; Lout_shape1[n_out] != list_end; n_out++ )
        {
            offset = Lout_shape1[n_out]*3;

            image_shape_uchar[offset] = image_phi_uchar[offset];
            image_shape_uchar[offset+1] = image_phi_uchar[offset+1];
            image_shape_uchar[offset+2] = image_phi_uchar[offset+2];
        }

        for( int n_in = 0; Lin_shape1[n_in] != list_end; n_in++ )
        {
            offset = Lin_shape1[n_in]*3;

            image_shape_uchar[offset] = image_phi_uchar[offset];
            image_shape_uchar[offset+1] = image_phi_uchar[offset+1];
            image_shape_uchar[offset+2] = image_phi_uchar[offset+2];
        }

        if( hasEllipse2 )
        {
            // Thanks to Alois Zingl for his ellipse Bresenham's Algorithm implementation
            // http://free.pages.at/easyfilter/bresenham.html

            int n = 0;
            int x0 = int((1.0-init_width2)*double(img_width)/2.0 + center_x2*double(img_width));
            int y0 = int((1.0-init_height2)*double(img_height)/2.0 + center_y2*double(img_height));
            int x1 = int((double(img_width)-(1.0-init_width2)*double(img_width)/2.0) + center_x2*double(img_width));
            int y1 = int((double(img_height)-(1.0-init_height2)*double(img_height)/2.0)+ center_y2*double(img_height));

            // values of diameter
            double a = abs(x1-x0);
            int b = abs(y1-y0);

            double b1 = b&1;
            double dx = double(4*(1-a)*b*b);
            double dy = double(4*(b1+1)*a*a);
            double err = dx+dy+double(b1*a*a); // error increment
            double e2; // error of 1.step

            if( x0 > x1 )
            {
                x0 = x1; // if called with swapped points
                x1 += int(a);
            }

            if( y0 > y1 )
            {
                y0 = y1; // .. exchange them
            }

            y0 += (b+1)/2;
            y1 = y0-int(b1); // starting pixel
            a *= 8.0*a;
            b1 = 8.0*double(b)*double(b);

            do
            {
                if( n < 2*img_size-20 )
                {
                    for( int ddy = -2; ddy <= 2; ddy++ )
                    {
                        for( int ddx = -2; ddx <= 2; ddx++ )
                        {
                            shape_points[n++] = x1+ddx+(y0+ddy)*img_width; //   I. Quadrant
                            shape_points[n++] = x0+ddx+(y0+ddy)*img_width; //  II. Quadrant
                            shape_points[n++] = x0+ddx+(y1+ddy)*img_width; // III. Quadrant
                            shape_points[n++] = x1+ddx+(y1+ddy)*img_width; //  IV. Quadrant
                        }
                    }
                }

                e2 = 2.0*err;

                // x step
                if( e2 >= dx )
                {
                    x0++;
                    x1--;
                    dx += b1;
                    err += dx;
                }

                // y step
                if( e2 <= dy )
                {
                    y0++;
                    y1--;
                    dy += a;
                    err += dy;
                }

            }
            while( x0 <= x1 );

            // too early stop of flat ellipses a = 1
            // -> finish tip of ellipse
            while( y0-y1 < b )
            {
                if( n < 2*img_size-20 )
                {
                    for( int ddy = -2; ddy <= 2; ddy++ )
                    {
                        for( int ddx = -2; ddx <= 2; ddx++ )
                        {
                            shape_points[n++] = x0-1+ddx+(y0+ddy)*img_width;
                            shape_points[n++] = x1+1+ddx+(y0+ddy)*img_width;
                            shape_points[n++] = x0-1+ddx+(y1+ddy)*img_width;
                            shape_points[n++] = x1+1+ddx+(y1+ddy)*img_width;
                        }
                    }
                    y0++;
                    y1--;
                }
            }

            // to mark the end;
            shape_points[n] = list_end;

            int n_out = 0;
            int n_in = 0;
            for( int n = 0; shape_points[n] != list_end; n++ )
            {
                offset = shape_points[n];

                y = offset/img_width;
                x = offset-y*img_width;

                if( x >= 0 && x < img_width && y >= 0 && y < img_height )
                {
                    if( (double(y)-(1.0+2.0*center_y2)*double(img_height)/2.0)*(double(y)-(1.0+2.0*center_y2)*double(img_height)/2.0)/(init_height2*double(img_height)/2.0*init_height2*double(img_height)/2.0)+(double(x)-(1.0+2.0*center_x2)*double(img_width)/2.0)*(double(x)-(1.0+2.0*center_x2)*double(img_width)/2.0)/(init_width2*double(img_width)/2.0*init_width2*double(img_width)/2.0) > 1.0 )
                    {
                        if( (double(y-1)-(1.0+2.0*center_y2)*double(img_height)/2.0)*(double(y-1)-(1.0+2.0*center_y2)*double(img_height)/2.0)/(init_height2*double(img_height)/2.0*init_height2*double(img_height)/2.0)+(double(x)-(1.0+2.0*center_x2)*double(img_width)/2.0)*(double(x)-(1.0+2.0*center_x2)*double(img_width)/2.0)/(init_width2*double(img_width)/2.0*init_width2*double(img_width)/2.0) <= 1.0 )
                        {
                            if( Lout_shape1[n_out] != offset )
                            {
                                Lout_shape1[n_out++] = offset;
                            }
                        }

                        if( (double(y)-(1.0+2.0*center_y2)*double(img_height)/2.0)*(double(y)-(1.0+2.0*center_y2)*double(img_height)/2.0)/(init_height2*double(img_height)/2.0*init_height2*double(img_height)/2.0)+(double(x-1)-(1.0+2.0*center_x2)*double(img_width)/2.0)*(double(x-1)-(1.0+2.0*center_x2)*double(img_width)/2.0)/(init_width2*double(img_width)/2.0*init_width2*double(img_width)/2.0) <= 1.0 )
                        {
                            if( Lout_shape1[n_out] != offset )
                            {
                                Lout_shape1[n_out++] = offset;
                            }
                        }

                        if( (double(y)-(1.0+2.0*center_y2)*double(img_height)/2.0)*(double(y)-(1.0+2.0*center_y2)*double(img_height)/2.0)/(init_height2*double(img_height)/2.0*init_height2*double(img_height)/2.0)+(double(x+1)-(1.0+2.0*center_x2)*double(img_width)/2.0)*(double(x+1)-(1.0+2.0*center_x2)*double(img_width)/2.0)/(init_width2*double(img_width)/2.0*init_width2*double(img_width)/2.0) <= 1.0 )
                        {
                            if( Lout_shape1[n_out] != offset )
                            {
                                Lout_shape1[n_out++] = offset;
                            }
                        }

                        if( (double(y+1)-(1.0+2.0*center_y2)*double(img_height)/2.0)*(double(y+1)-(1.0+2.0*center_y2)*double(img_height)/2.0)/(init_height2*double(img_height)/2.0*init_height2*double(img_height)/2.0)+(double(x)-(1.0+2.0*center_x2)*double(img_width)/2.0)*(double(x)-(1.0+2.0*center_x2)*double(img_width)/2.0)/(init_width2*double(img_width)/2.0*init_width2*double(img_width)/2.0) <= 1.0 )
                        {
                            if( Lout_shape1[n_out] != offset )
                            {
                                Lout_shape1[n_out++] = offset;
                            }
                        }
                    }
                    else
                    {
                        if( (double(y-1)-(1.0+2.0*center_y2)*double(img_height)/2.0)*(double(y-1)-(1.0+2.0*center_y2)*double(img_height)/2.0)/(init_height2*double(img_height)/2.0*init_height2*double(img_height)/2.0)+(double(x)-(1.0+2.0*center_x2)*double(img_width)/2.0)*(double(x)-(1.0+2.0*center_x2)*double(img_width)/2.0)/(init_width2*double(img_width)/2.0*init_width2*double(img_width)/2.0) > 1.0 )
                        {
                            if( Lin_shape1[n_in] != offset )
                            {
                                Lin_shape1[n_in++] = offset;
                            }
                        }

                        if( (double(y)-(1.0+2.0*center_y2)*double(img_height)/2.0)*(double(y)-(1.0+2.0*center_y2)*double(img_height)/2.0)/(init_height2*double(img_height)/2.0*init_height2*double(img_height)/2.0)+(double(x-1)-(1.0+2.0*center_x2)*double(img_width)/2.0)*(double(x-1)-(1.0+2.0*center_x2)*double(img_width)/2.0)/(init_width2*double(img_width)/2.0*init_width2*double(img_width)/2.0) > 1.0 )
                        {
                            if( Lin_shape1[n_in] != offset )
                            {
                                Lin_shape1[n_in++] = offset;
                            }
                        }

                        if( (double(y)-(1.0+2.0*center_y2)*double(img_height)/2.0)*(double(y)-(1.0+2.0*center_y2)*double(img_height)/2.0)/(init_height2*double(img_height)/2.0*init_height2*double(img_height)/2.0)+(double(x+1)-(1.0+2.0*center_x2)*double(img_width)/2.0)*(double(x+1)-(1.0+2.0*center_x2)*double(img_width)/2.0)/(init_width2*double(img_width)/2.0*init_width2*double(img_width)/2.0) > 1.0 )
                        {
                            if( Lin_shape1[n_in] != offset )
                            {
                                Lin_shape1[n_in++] = offset;
                            }
                        }

                        if( (double(y+1)-(1.0+2.0*center_y2)*double(img_height)/2.0)*(double(y+1)-(1.0+2.0*center_y2)*double(img_height)/2.0)/(init_height2*double(img_height)/2.0*init_height2*double(img_height)/2.0)+(double(x)-(1.0+2.0*center_x2)*double(img_width)/2.0)*(double(x)-(1.0+2.0*center_x2)*double(img_width)/2.0)/(init_width2*double(img_width)/2.0*init_width2*double(img_width)/2.0) > 1.0 )
                        {
                            if( Lin_shape1[n_in] != offset )
                            {
                                Lin_shape1[n_in++] = offset;
                            }
                        }
                    }
                }

            }

            Lout_shape1[n_out] = list_end;
            Lin_shape1[n_in] = list_end;
        }
        else
        {
            int x1, y1;

            int n = 0;
            for( x = int((1.0-init_width2)*double(img_width)/2.0 + center_x2*double(img_width)); x < int((double(img_width)-(1.0-init_width2)*double(img_width)/2.0) + center_x2*double(img_width)); x++ )
            {
                y = int((1.0-init_height2)*double(img_height)/2.0 + center_y2*double(img_height));
                y1 = int((double(img_height)-(1.0-init_height2)*double(img_height)/2.0)+ center_y2*double(img_height));

                if( n < 2*img_size-10 )
                {
                    for( int dy = -2; dy <= 2; dy++ )
                    {
                        for( int dx = -2; dx <= 2; dx++ )
                        {
                            shape_points[n++] = x+dx+(y+dy)*img_width;
                            shape_points[n++] = x+dx+(y1+dy)*img_width;
                        }
                    }
                }
            }

            for( y = int((1.0-init_height2)*double(img_height)/2.0 + center_y2*double(img_height)); y < int((double(img_height)-(1.0-init_height2)*double(img_height)/2.0)+ center_y2*double(img_height)); y++ )
            {
                x = int((1.0-init_width2)*double(img_width)/2.0 + center_x2*double(img_width));

                x1 = int((double(img_width)-(1.0-init_width2)*double(img_width)/2.0) + center_x2*double(img_width));

                if( n < 2*img_size-10 )
                {
                    for( int dy = -2; dy <= 2; dy++ )
                    {
                        for( int dx = -2; dx <= 2; dx++ )
                        {
                            shape_points[n++] = x+dx+(y+dy)*img_width;
                            shape_points[n++] = x1+dx+(y+dy)*img_width;
                        }
                    }
                }
            }

            // to mark the end;
            shape_points[n] = list_end;

            int n_out = 0;
            int n_in = 0;
            for( int n = 0; shape_points[n] != list_end; n++ )
            {
                offset = shape_points[n];

                y = offset/img_width;
                x = offset-y*img_width;

                if( x >= 0 && x < img_width && y >= 0 && y < img_height )
                {
                    if( double(y) > ((1.0-init_height2)*double(img_height)/2.0 + center_y2*double(img_height)) && double(y) < ((double(img_height)-(1.0-init_height2)*double(img_height)/2.0) + center_y2*double(img_height)) && double(x) > ((1.0-init_width2)*double(img_width)/2.0 + center_x2*double(img_width)) && double(x) < ((double(img_width)-(1.0-init_width2)*double(img_width)/2.0) + center_x2*double(img_width)) )
                    {
                        /*for( int dy = -1; dy <= 1; dy++ )
                        {
                            for( int dx = -1; dx <= 1; dx++ )
                            {
                                if( !( dx == 0 && dy == 0 ) )
                                {
                                    if( !( double(y+dy) > ((1.0-init_height2)*double(img_height)/2.0 + center_y2*double(img_height)) && double(y+dy) < ((double(img_height)-(1.0-init_height2)*double(img_height)/2.0) + center_y2*double(img_height)) && double(x+dx) > ((1.0-init_width2)*double(img_width)/2.0 + center_x2*double(img_width)) && double(x+dx) < ((double(img_width)-(1.0-init_width2)*double(img_width)/2.0) + center_x2*double(img_width)) ) )
                                    {
                                        if( Lin_shape1[n_in] != i )
                                        {
                                            Lin_shape1[n_in++] = i;
                                        }
                                    }
                                }
                            }
                        }*/

                        if( !( double(y-1) > ((1.0-init_height2)*double(img_height)/2.0 + center_y2*double(img_height)) && double(y-1) < ((double(img_height)-(1.0-init_height2)*double(img_height)/2.0) + center_y2*double(img_height)) && double(x) > ((1.0-init_width2)*double(img_width)/2.0 + center_x2*double(img_width)) && double(x) < ((double(img_width)-(1.0-init_width2)*double(img_width)/2.0) + center_x2*double(img_width)) ) )
                        {
                            if( Lin_shape1[n_in] != offset )
                            {
                                Lin_shape1[n_in++] = offset;
                            }
                        }

                        if( !( double(y) > ((1.0-init_height2)*double(img_height)/2.0 + center_y2*double(img_height)) && double(y) < ((double(img_height)-(1.0-init_height2)*double(img_height)/2.0) + center_y2*double(img_height)) && double(x-1) > ((1.0-init_width2)*double(img_width)/2.0 + center_x2*double(img_width)) && double(x-1) < ((double(img_width)-(1.0-init_width2)*double(img_width)/2.0) + center_x2*double(img_width)) ) )
                        {
                            if( Lin_shape1[n_in] != offset )
                            {
                                Lin_shape1[n_in++] = offset;
                            }
                        }

                        if( !( double(y) > ((1.0-init_height2)*double(img_height)/2.0 + center_y2*double(img_height)) && double(y) < ((double(img_height)-(1.0-init_height2)*double(img_height)/2.0) + center_y2*double(img_height)) && double(x+1) > ((1.0-init_width2)*double(img_width)/2.0 + center_x2*double(img_width)) && double(x+1) < ((double(img_width)-(1.0-init_width2)*double(img_width)/2.0) + center_x2*double(img_width)) ) )
                        {
                            if( Lin_shape1[n_in] != offset )
                            {
                                Lin_shape1[n_in++] = offset;
                            }
                        }

                        if( !( double(y+1) > ((1.0-init_height2)*double(img_height)/2.0 + center_y2*double(img_height)) && double(y+1) < ((double(img_height)-(1.0-init_height2)*double(img_height)/2.0) + center_y2*double(img_height)) && double(x) > ((1.0-init_width2)*double(img_width)/2.0 + center_x2*double(img_width)) && double(x) < ((double(img_width)-(1.0-init_width2)*double(img_width)/2.0) + center_x2*double(img_width)) ) )
                        {
                            if( Lin_shape1[n_in] != offset )
                            {
                                Lin_shape1[n_in++] = offset;
                            }
                        }
                    }
                    else
                    {
                        /*for( int dy = -1; dy <= 1; dy++ )
                        {
                            for( int dx = -1; dx <= 1; dx++ )
                            {
                                if( !( dx == 0 && dy == 0 ) )
                                {
                                    if( double(y+dy) > ((1.0-init_height2)*double(img_height)/2.0 + center_y2*double(img_height)) && double(y+dy) < ((double(img_height)-(1.0-init_height2)*double(img_height)/2.0) + center_y2*double(img_height)) && double(x+dx) > ((1.0-init_width2)*double(img_width)/2.0 + center_x2*double(img_width)) && double(x+dx) < ((double(img_width)-(1.0-init_width2)*double(img_width)/2.0) + center_x2*double(img_width)) )
                                    {
                                        if( Lout_shape1[n_out] != i )
                                        {
                                            Lout_shape1[n_out++] = i;
                                        }
                                    }
                                }
                            }
                        }*/

                        if( double(y-1) > ((1.0-init_height2)*double(img_height)/2.0 + center_y2*double(img_height)) && double(y-1) < ((double(img_height)-(1.0-init_height2)*double(img_height)/2.0) + center_y2*double(img_height)) && double(x) > ((1.0-init_width2)*double(img_width)/2.0 + center_x2*double(img_width)) && double(x) < ((double(img_width)-(1.0-init_width2)*double(img_width)/2.0) + center_x2*double(img_width)) )
                        {
                            if( Lout_shape1[n_out] != offset )
                            {
                                Lout_shape1[n_out++] = offset;
                            }
                        }

                        if( double(y) > ((1.0-init_height2)*double(img_height)/2.0 + center_y2*double(img_height)) && double(y) < ((double(img_height)-(1.0-init_height2)*double(img_height)/2.0) + center_y2*double(img_height)) && double(x-1) > ((1.0-init_width2)*double(img_width)/2.0 + center_x2*double(img_width)) && double(x-1) < ((double(img_width)-(1.0-init_width2)*double(img_width)/2.0) + center_x2*double(img_width)) )
                        {
                            if( Lout_shape1[n_out] != offset )
                            {
                                Lout_shape1[n_out++] = offset;
                            }
                        }

                        if( double(y) > ((1.0-init_height2)*double(img_height)/2.0 + center_y2*double(img_height)) && double(y) < ((double(img_height)-(1.0-init_height2)*double(img_height)/2.0) + center_y2*double(img_height)) && double(x+1) > ((1.0-init_width2)*double(img_width)/2.0 + center_x2*double(img_width)) && double(x+1) < ((double(img_width)-(1.0-init_width2)*double(img_width)/2.0) + center_x2*double(img_width)) )
                        {
                            if( Lout_shape1[n_out] != offset )
                            {
                                Lout_shape1[n_out++] = offset;
                            }
                        }

                        if( double(y+1) > ((1.0-init_height2)*double(img_height)/2.0 + center_y2*double(img_height)) && double(y+1) < ((double(img_height)-(1.0-init_height2)*double(img_height)/2.0) + center_y2*double(img_height)) && double(x) > ((1.0-init_width2)*double(img_width)/2.0 + center_x2*double(img_width)) && double(x) < ((double(img_width)-(1.0-init_width2)*double(img_width)/2.0) + center_x2*double(img_width)) )
                        {
                            if( Lout_shape1[n_out] != offset )
                            {
                                Lout_shape1[n_out++] = offset;
                            }
                        }
                    }
                }

            }

            Lout_shape1[n_out] = list_end;
            Lin_shape1[n_in] = list_end;
        }

        if( outsidecolor_combobox->currentIndex() != 9 )
        {
            for( int n_out = 0; Lout_shape1[n_out] != list_end; n_out++ )
            {
                offset = Lout_shape1[n_out]*3;

                image_shape_uchar[offset] = Rout2;
                image_shape_uchar[offset+1] = Gout2;
                image_shape_uchar[offset+2] = Bout2;
            }
        }

        if( insidecolor_combobox->currentIndex() != 9 )
        {
            for( int n_in = 0; Lin_shape1[n_in] != list_end; n_in++ )
            {
                offset = Lin_shape1[n_in]*3;

                image_shape_uchar[offset] = Rin2;
                image_shape_uchar[offset+1] = Gin2;
                image_shape_uchar[offset+2] = Bin2;
            }
        }

        imageLabel_settings->set_qimage(image_shape);
    }
    return;
}

// Surcharge pour avoir une signature identique (memes paramètres) entre signaux et slots de Qt
void ImageViewer::shape_visu(int value)
{
    shape_visu();
}


// Remet phi_init2_clean a zéro, c'est à dire tout correspond à l'extérieur
void ImageViewer::clean_phi_visu()
{
    if( phi_init2 != NULL && phi_init2_clean != NULL )
    {
        clean_boundaries(phi_init2, phi_init2_clean);
        for( int offset = 0; offset < img_size; offset++ )
        {
            phi_init2[offset] = 1;
            phi_init2_clean[offset] = 3;
        }
        clean_boundaries(phi_init2, phi_init2_clean);

        shape_visu();

    }
}

// Soustrait une forme à phi_init2
void ImageViewer::phi_subtract_shape()
{
    if( shape != NULL && phi_init2 != NULL )
    {

        int offset, x, y;

        if( hasEllipse2 )
        {
            for( offset = 0; offset < img_size; offset++ )
            {
                y = offset/img_width;
                x = offset-y*img_width;

                // inéquation de l'ellipse
                // grand axe et petit axe proportionnel à la largeur et la hauteur de l'image
                // pour une image carrée, l'ellipse est un cercle
                // variable init_shape_size par défaut a 0.95 (entre 0 et 1 sinon)
                if( (double(y)-(1.0+2.0*center_y2)*double(img_height)/2.0)*(double(y)-(1.0+2.0*center_y2)*double(img_height)/2.0)/(init_height2*double(img_height)/2.0*init_height2*double(img_height)/2.0)+(double(x)-(1.0+2.0*center_x2)*double(img_width)/2.0)*(double(x)-(1.0+2.0*center_x2)*double(img_width)/2.0)/(init_width2*double(img_width)/2.0*init_width2*double(img_width)/2.0) > 1.0 )
                {
                    shape[offset] = 1;
                }
                else
                {
                    shape[offset] = -1;
                }
            }

        }

        else
        {
            for( offset = 0; offset < img_size; offset++ )
            {
                y = offset/img_width;
                x = offset-y*img_width;

                // pour faire un rectangle
                // largeur et hauteur du rectangle proportionnel à la largeur et la hauteur de l'image
                // pour une image carrée, le rectangle est un carré
                if( double(y) > ((1.0-init_height2)*double(img_height)/2.0 + center_y2*double(img_height)) && double(y) < ((double(img_height)-(1.0-init_height2)*double(img_height)/2.0) + center_y2*double(img_height)) && double(x) > ((1.0-init_width2)*double(img_width)/2.0 + center_x2*double(img_width)) && double(x) < ((double(img_width)-(1.0-init_width2)*double(img_width)/2.0) + center_x2*double(img_width)) )
                {
                    shape[offset] = -1;
                }
                else
                {
                    shape[offset] = 1;
                }
            }

        }

        bool phi_init2_modif = false;

        for( offset = 0; offset < img_size; offset++ )
        {
            if( shape[offset] == -1 && phi_init2[offset] == -1 )
            {
                phi_init2[offset] = 1;
                phi_init2_modif = true;
            }
        }

        clean_boundaries(phi_init2, phi_init2_clean);

        if( !phi_init2_modif )
        {
            bool hasOne = false;
            bool hasMinusOne = false;

            for( offset = 0; offset < img_size; offset++ )
            {
                if( shape[offset] == 1 )
                {
                    hasOne = true;
                }
                if( shape[offset] == -1 )
                {
                    hasMinusOne = true;
                }
            }

            if( hasOne && hasMinusOne )
            {
                phi_add_shape();
            }
        }
    }

    return;
}

// Ajoute une forme à phi_init2
void ImageViewer::phi_add_shape()
{
    if( shape != NULL && phi_init2 != NULL )
    {
        int offset, x, y;

        if( hasEllipse2 )
        {
            for( offset = 0; offset < img_size; offset++ )
            {
                y = offset/img_width;
                x = offset-y*img_width;

                // inéquation de l'ellipse
                // grand axe et petit axe proportionnel à la largeur et la hauteur de l'image
                // pour une image carrée, l'ellipse est un cercle
                // variable init_shape_size par défaut a 0.95 (entre 0 et 1 sinon)
                if( (double(y)-(1.0+2.0*center_y2)*double(img_height)/2.0)*(double(y)-(1.0+2.0*center_y2)*double(img_height)/2.0)/(init_height2*double(img_height)/2.0*init_height2*double(img_height)/2.0)+(double(x)-(1.0+2.0*center_x2)*double(img_width)/2.0)*(double(x)-(1.0+2.0*center_x2)*double(img_width)/2.0)/(init_width2*double(img_width)/2.0*init_width2*double(img_width)/2.0) > 1.0 )
                {
                    shape[offset] = 1;
                }
                else
                {
                    shape[offset] = -1;
                }
            }
        }

        else
        {
            for( offset = 0; offset < img_size; offset++ )
            {
                y = offset/img_width;
                x = offset-y*img_width;

                // pour faire un rectangle
                // largeur et hauteur du rectangle proportionnel à la largeur et la hauteur de l'image
                // pour une image carrée, le rectangle est un carré
                if( double(y) > ((1.0-init_height2)*double(img_height)/2.0 + center_y2*double(img_height)) && double(y) < ((double(img_height)-(1.0-init_height2)*double(img_height)/2.0) + center_y2*double(img_height)) && double(x) > ((1.0-init_width2)*double(img_width)/2.0 + center_x2*double(img_width)) && double(x) < ((double(img_width)-(1.0-init_width2)*double(img_width)/2.0) + center_x2*double(img_width)) )
                {
                    shape[offset] = -1;
                }
                else
                {
                    shape[offset] = 1;
                }
            }
        }

        bool phi_init2_modif = false;

        for( offset = 0; offset < img_size; offset++ )
        {
            if( shape[offset] == -1 && phi_init2[offset] == 1 )
            {
                phi_init2[offset] = -1;
                phi_init2_modif = true;
            }
        }

        clean_boundaries(phi_init2, phi_init2_clean);

        if( !phi_init2_modif )
        {
            bool hasOne = false;
            bool hasMinusOne = false;

            for( offset = 0; offset < img_size; offset++ )
            {
                if( shape[offset] == 1 )
                {
                    hasOne = true;
                }
                if( shape[offset] == -1 )
                {
                    hasMinusOne = true;
                }
            }

            if( hasOne && hasMinusOne )
            {
                phi_subtract_shape();
            }
        }
    }

    return;
}

// affiche l'image+phi en clair dans la fenêtre de configuration
// dans l'onglet initialisation, affiche l'image+phi en foncé (booléen d'entrée a true) lorsque qu'on clique sur l'image
// ou qu'on appuie sur les bouttons add subtract
void ImageViewer::phi_visu(bool dark_color)
{
    if( img1 != NULL )
    {
        if( outsidecolor_combobox->currentIndex() == 8 )
        {
            Rout2 = Rout_selected2;
            Gout2 = Gout_selected2;
            Bout2 = Bout_selected2;
        }
        else
        {
            get_color(outsidecolor_combobox->currentIndex(),Rout2,Gout2,Bout2);
        }

        if( insidecolor_combobox->currentIndex() == 8 )
        {
            Rin2 = Rin_selected2;
            Gin2 = Gin_selected2;
            Bin2 = Bin_selected2;
        }
        else
        {
            get_color(insidecolor_combobox->currentIndex(),Rin2,Gin2,Bin2);
        }


        int offset;

        if( dark_color )
        {
            if( outsidecolor_combobox->currentIndex() != 9 )
            {
                for( int n_out = 0; Lout_2[n_out] != list_end; n_out++ )
                {
                    offset = Lout_2[n_out]*3;

                    image_phi_uchar[offset] = Rout2/2;
                    image_phi_uchar[offset+1] = Gout2/2;
                    image_phi_uchar[offset+2] = Bout2/2;

                    image_shape_uchar[offset] = Rout2/2;
                    image_shape_uchar[offset+1] = Gout2/2;
                    image_shape_uchar[offset+2] = Bout2/2;
                }
            }

            if( insidecolor_combobox->currentIndex() != 9 )
            {
                for( int n_in = 0; Lin_2[n_in] != list_end; n_in++ )
                {
                    offset = Lin_2[n_in]*3;

                    image_phi_uchar[offset] = Rin2/2;
                    image_phi_uchar[offset+1] = Gin2/2;
                    image_phi_uchar[offset+2] = Bin2/2;

                    image_shape_uchar[offset] = Rin2/2;
                    image_shape_uchar[offset+1] = Gin2/2;
                    image_shape_uchar[offset+2] = Bin2/2;
                }
            }
        }


        else
        {
            if( outsidecolor_combobox->currentIndex() != 9 )
            {
                for( int n_out = 0; Lout_2[n_out] != list_end; n_out++ )
                {
                    offset = Lout_2[n_out]*3;

                    image_phi_uchar[offset] = Rout2;
                    image_phi_uchar[offset+1] = Gout2;
                    image_phi_uchar[offset+2] = Bout2;

                    image_shape_uchar[offset] = Rout2/2;
                    image_shape_uchar[offset+1] = Gout2/2;
                    image_shape_uchar[offset+2] = Bout2/2;
                }
            }
            else
            {
                // efface l'affichage des listes
                for( int n_out = 0; Lout_2[n_out] != list_end; n_out++ )
                {
                    offset = Lout_2[n_out]*3;

                    image_phi_uchar[offset] = image_filter_uchar[offset];
                    image_phi_uchar[offset+1] = image_filter_uchar[offset+1];
                    image_phi_uchar[offset+2] = image_filter_uchar[offset+2];

                    image_shape_uchar[offset] = image_filter_uchar[offset];
                    image_shape_uchar[offset+1] = image_filter_uchar[offset+1];
                    image_shape_uchar[offset+2] = image_filter_uchar[offset+2];
                }
            }

            if( insidecolor_combobox->currentIndex() != 9 )
            {
                for( int n_in = 0; Lin_2[n_in] != list_end; n_in++ )
                {
                    offset = Lin_2[n_in]*3;

                    image_phi_uchar[offset] = Rin2;
                    image_phi_uchar[offset+1] = Gin2;
                    image_phi_uchar[offset+2] = Bin2;

                    image_shape_uchar[offset] = Rin2/2;
                    image_shape_uchar[offset+1] = Gin2/2;
                    image_shape_uchar[offset+2] = Bin2/2;
                }
            }
            else
            {
                for( int n_in = 0; Lin_2[n_in] != list_end; n_in++ )
                {
                    offset = Lin_2[n_in]*3;

                    image_phi_uchar[offset] = image_filter_uchar[offset];
                    image_phi_uchar[offset+1] = image_filter_uchar[offset+1];
                    image_phi_uchar[offset+2] = image_filter_uchar[offset+2];

                    image_shape_uchar[offset] = image_filter_uchar[offset];
                    image_shape_uchar[offset+1] = image_filter_uchar[offset+1];
                    image_shape_uchar[offset+2] = image_filter_uchar[offset+2];
                }
            }
        }

        if( hasContoursHidden && onglets->currentIndex() == 2 )
        {
            // affiche l'ellipse ou le rectangle
            imageLabel_settings->set_qimage(image_filter);
        }
        else
        {
           // affiche l'ellipse ou le rectangle
           imageLabel_settings->set_qimage(image_phi);
        }

        // efface l'ellipse ou le recangle de la QImage
        // efface les listes de la QImage
        for( int n_out = 0; Lout_shape1[n_out] != list_end; n_out++ )
        {
            offset = Lout_shape1[n_out]*3;

            image_shape_uchar[offset] = image_phi_uchar[offset];
            image_shape_uchar[offset+1] = image_phi_uchar[offset+1];
            image_shape_uchar[offset+2] = image_phi_uchar[offset+2];
        }

        for( int n_in = 0; Lin_shape1[n_in] != list_end; n_in++ )
        {
            offset = Lin_shape1[n_in]*3;

            image_shape_uchar[offset] = image_phi_uchar[offset];
            image_shape_uchar[offset+1] = image_phi_uchar[offset+1];
            image_shape_uchar[offset+2] = image_phi_uchar[offset+2];
        }
    }

    return;
}

void ImageViewer::get_color(int index, unsigned char& R, unsigned char& G, unsigned char& B)
{

    if( index == 0 )
    {
        R = 255;
        G = 0;
        B = 0;
    }
    if( index == 1 )
    {
        R = 0;
        G = 255;
        B = 0;
    }
    if( index == 2 )
    {
        R = 0;
        G = 0;
        B = 255;
    }
    if( index == 3 )
    {
        R = 0;
        G = 255;
        B = 255;
    }
    if( index == 4 )
    {
        R = 255;
        G = 0;
        B = 255;
    }
    if( index == 5 )
    {
        R = 255;
        G = 255;
        B = 0;
    }
    if( index == 6 || index == 9 )
    {
        R = 0;
        G = 0;
        B = 0;
    }
    if( index == 7 )
    {
        R = 255;
        G = 255;
        B = 255;
    }

    return;
}

// Surcharge de phi_visu pour que le signal et le slot de Qt aient la même signature (les memes types de paramètres)
// pour les combo box couleurs
void ImageViewer::phi_visu(int value)
{
    phi_visu(false);
    return;
}

// Fonction appelée quand on clique sur le boutton add dans l'onglet initialization
// ou par clic gauche
void ImageViewer::add_visu()
{
    // ajoute
    phi_add_shape();
    // visualisation de phi en foncé (booléen true)
    phi_visu(true);
    return;
}

// Fonction appelée quand on clique sur le boutton subtract dans l'onglet initialization
// ou par clic droit
void ImageViewer::subtract_visu()
{
    // soustrait
    phi_subtract_shape();
    // visualisation de phi en foncé (booléen true)
    phi_visu(true);
    return;
}

// Fonction appelée quand on clique sur save
void ImageViewer::saveImage()
{
    QFileInfo fi(fileName);
    QString fileName_save;

    // selection du chemin à partir d'une boîte de dialogue
    if( iteration1 == -1 )
    {
        fileName_save = QFileDialog::getSaveFileName(this,
                                                     tr("Save the image displayed by the main window"),
                                                     last_directory_used + "/" + fi.baseName(),
                                                     "BMP (*.bmp);;JPG (*.jpg);;PNG (*.png);;PPM (*.ppm);;TIFF (*.tiff);;XBM (*.xbm);;XPM (*.xpm)");
    }
    else
    {
        show_phi_list_value();

        if( image_disp == 1 )
        {
            fileName_save = QFileDialog::getSaveFileName(this,
                                                         tr("Save the image displayed by the main window"),
                                                         last_directory_used + "/" + fi.baseName() + tr("_with_phi(t=")+QString::number(iteration1)+")",
                                                         "BMP (*.bmp);;JPG (*.jpg);;PNG (*.png);;PPM (*.ppm);;TIFF (*.tiff);;XBM (*.xbm);;XPM (*.xpm)");
        }
        if( image_disp == 2 )
        {

            if( hasGaussianNoise || hasSaltNoise || hasSpeckleNoise || hasMeanFilt || hasGaussianFilt || hasMedianFilt || hasAnisoDiff || hasOpenFilt || hasCloseFilt || hasTophatFilt || model == 2 )
            {

                fileName_save = QFileDialog::getSaveFileName(this,
                                                            tr("Save the image displayed by the main window"),
                                                            last_directory_used + "/" + tr("preprocessed_") + fi.baseName() + tr("_with_phi(t=")+QString::number(iteration1)+")",
                                                            "BMP (*.bmp);;JPG (*.jpg);;PNG (*.png);;PPM (*.ppm);;TIFF (*.tiff);;XBM (*.xbm);;XPM (*.xpm)");

            }
            else
            {
                fileName_save = QFileDialog::getSaveFileName(this,
                                                             tr("Save the image displayed by the main window"),
                                                             last_directory_used + "/" + fi.baseName()+tr("_with_phi(t=") + QString::number(iteration1)+")",
                                                             "BMP (*.bmp);;JPG (*.jpg);;PNG (*.png);;PPM (*.ppm);;TIFF (*.tiff);;XBM (*.xbm);;XPM (*.xpm)");
            }
        }
    }

    QImage img_displayed = imageLabel->get_qimage();

    if( !fileName_save.isEmpty() )
    {
        QFileInfo fi1(fileName_save);
        last_directory_used = fi1.absolutePath();


        img_displayed.save(fileName_save);
    }

    if( (hasGaussianNoise || hasSaltNoise || hasSpeckleNoise || hasMeanFilt || hasGaussianFilt || hasMedianFilt || hasAnisoDiff || hasOpenFilt || hasCloseFilt || hasTophatFilt || model == 2) && !image_save_preprocess.isNull() )
    {
        // sélection du chemin+nom de sauvegarde à partir d'une boîte de dialogue
        QString fileName_save2 = QFileDialog::getSaveFileName(this,
                                                              tr("Save the preprocessed image"),
                                                              last_directory_used + "/" + tr("preprocessed_") + fi.baseName(),
                                                              "BMP (*.bmp);;JPG (*.jpg);;PNG (*.png);;PPM (*.ppm);;TIFF (*.tiff);;XBM (*.xbm);;XPM (*.xpm)");


        if( !fileName_save2.isEmpty() )
        {
            QFileInfo fi2(fileName_save2);
            last_directory_used = fi2.absolutePath();

            image_save_preprocess.save(fileName_save2);
        }
    }
}

// Fonction appelée par le boutton selected de boundaries outside
// sélection et affichage d'une couleur particulière
void ImageViewer::color_out()
{
    // Sélection d'une QColor a partir d'une boite de dialogue couleur
    QColor color_out = QColorDialog::getColor(Qt::white, this, tr("Select Lout color"));
    if( color_out.isValid() )
    {
        Rout_selected2 = (unsigned char)(color_out.red());
        Gout_selected2 = (unsigned char)(color_out.green());
        Bout_selected2 = (unsigned char)(color_out.blue());

        QPixmap pm(12,12);
        pm.fill(color_out);
        outsidecolor_combobox->setItemIcon(8,pm);

        outsidecolor_combobox->setCurrentIndex(8);
    }
    // affichage de l'image avec la nouvelle couleur sélectionnée
    phi_visu(false);

}

// Fonction appelée par le boutton selected de boundaries inside
// sélection et affichage d'une couleure particulière
void ImageViewer::color_in()
{
    // Selection d'une QColor à partir d'une boîte de dialogue couleur
    QColor color_in = QColorDialog::getColor(Qt::white, this, tr("Select Lin color"));
    if( color_in.isValid() )
    {
        Rin_selected2 = (unsigned char)(color_in.red());
        Gin_selected2 = (unsigned char)(color_in.green());
        Bin_selected2 = (unsigned char)(color_in.blue());

        QPixmap pm(12,12);
        pm.fill(color_in);
        insidecolor_combobox->setItemIcon(8,pm);

        insidecolor_combobox->setCurrentIndex(8);
    }
    // affichage de l'image avec la nouvelle couleur sélectionnée
    phi_visu(false);
}

// Fonction appelée par tous les widgets de l'onglet Preprocessing
// Calcule et affiche l'image prétraitéé
void ImageViewer::filtering_visu()
{
    if( img1 != NULL && filters2 != NULL )
    {
        // récupération de tous les états des widgets de l'onglet preprocessing

        if( preprocessing_groupbox->isChecked() )
        {
            hasPreprocess2 = true;
        }
        else
        {
            hasPreprocess2 = false;
        }

        if( gaussian_noise_groupbox->isChecked() )
        {
            hasGaussianNoise2 = true;
        }
        else
        {
            hasGaussianNoise2 = false;
        }
        std_noise2 = std_noise_spin->value();

        if( salt_noise_groupbox->isChecked() )
        {
            hasSaltNoise2 = true;
        }
        else
        {
            hasSaltNoise2 = false;
        }
        proba_noise2 = proba_noise_spin->value()/100.0;

        if( speckle_noise_groupbox->isChecked() )
        {
            hasSpeckleNoise2 = true;
        }
        else
        {
            hasSpeckleNoise2 = false;
        }
        std_speckle_noise2 = std_speckle_noise_spin->value();

        if( median_groupbox->isChecked() )
        {
            hasMedianFilt2 = true;
        }
        else
        {
            hasMedianFilt2 = false;
        }
        kernel_median_length2 = klength_median_spin->value();
        if( complex_radio2->isChecked() )
        {
            hasO1algo2 = true;
        }
        else
        {
            hasO1algo2 = false;
        }

        if( mean_groupbox->isChecked() )
        {
            hasMeanFilt2 = true;
        }
        else
        {
            hasMeanFilt2 = false;
        }
        kernel_mean_length2 = klength_mean_spin->value();

        if( gaussian_groupbox->isChecked() )
        {
            hasGaussianFilt2 = true;
        }
        else
        {
            hasGaussianFilt2 = false;
        }
        kernel_gaussian_length2 = klength_gaussian_spin->value();
        sigma2 = std_filter_spin->value();


        if( aniso_groupbox->isChecked() )
        {
            hasAnisoDiff2 = true;
        }
        else
        {
            hasAnisoDiff2 = false;
        }

        max_itera2 = iteration_filter_spin->value();
        lambda2 = lambda_spin->value();
        kappa2 = kappa_spin->value();
        if( aniso1_radio->isChecked() )
        {
            aniso_option2 = 1;
        }
        if( aniso2_radio->isChecked() )
        {
            aniso_option2 = 2;
        }

        if( open_groupbox->isChecked() )
        {
            hasOpenFilt2 = true;
        }
        else
        {
            hasOpenFilt2 = false;
        }
        kernel_open_length2 = klength_open_spin->value();

        if( close_groupbox->isChecked() )
        {
            hasCloseFilt2 = true;
        }
        else
        {
            hasCloseFilt2 = false;
        }
        kernel_close_length2 = klength_close_spin->value();

        if( tophat_groupbox->isChecked() )
        {
            hasTophatFilt2 = true;
        }
        else
        {
            hasTophatFilt2 = false;
        }
        if( whitetophat_radio->isChecked() )
        {
            isWhiteTophat2 = true;
        }
        else
        {
            isWhiteTophat2 = false;
        }
        kernel_tophat_length2 = klength_tophat_spin->value();

        if( complex2_morpho_radio->isChecked() )
        {
            hasO1morpho2 = true;
        }
        else
        {
            hasO1morpho2 = false;
        }

        if( chanvese_radio->isChecked() )
        {
            model2 = 1;
        }
        if( geodesic_radio->isChecked() )
        {
            model2 = 2;
        }
        kernel_gradient_length2 = klength_gradient_spin->value();

        alpha2 = Yspin->value();
        beta2 = Uspin->value();
        gamma2 = Vspin->value();

        if( histo_checkbox->isChecked() )
        {
            hasHistoNormaliz2 = true;
        }
        else
        {
            hasHistoNormaliz2 = false;
        }


        if( hasOpenFilt2 || hasCloseFilt2 || hasTophatFilt2 )
        {
            algo_groupbox->setEnabled(true);
        }
        else
        {
            algo_groupbox->setEnabled(false);
        }



        // pour refiltrer à partir de l'image de départ
        filters2->initialyze_filtered();

        double elapsedTime;
        std::clock_t stopTime;
        std::clock_t startTime;

        startTime = std::clock();

        if( hasPreprocess2 )
        {
            if( hasGaussianNoise2 )
            {
                filters2->gaussian_white_noise(std_noise2);
            }
            if( hasSaltNoise2 )
            {
                filters2->salt_pepper_noise(proba_noise2);
            }
            if( hasSpeckleNoise2 )
            {
                filters2->speckle(std_speckle_noise2);
            }

            if( hasMeanFilt2 )
            {
                filters2->mean_filtering(kernel_mean_length2);
            }
            if( hasGaussianFilt2 )
            {
                filters2->gaussian_filtering(kernel_gaussian_length2, sigma2);
            }

            if( hasMedianFilt2 )
            {
                if( hasO1algo2 )
                {
                    filters2->median_filtering_o1(kernel_median_length2);
                }
                else
                {
                    filters2->median_filtering_oNlogN(kernel_median_length2);
                }
            }
            if( hasAnisoDiff2 )
            {
                filters2->anisotropic_diffusion(max_itera2, lambda2, kappa2, aniso_option2);
            }

            if( hasOpenFilt2 )
            {
                if( hasO1morpho2 )
                {
                    filters2->opening_o1(kernel_open_length2);
                }
                else
                {
                    filters2->opening(kernel_open_length2);
                }
            }

            if( hasCloseFilt2 )
            {
                if( hasO1morpho2 )
                {
                    filters2->closing_o1(kernel_close_length2);
                }
                else
                {
                    filters2->closing(kernel_close_length2);
                }
            }

            if( hasTophatFilt2 )
            {
                if( isWhiteTophat2 )
                {
                    if( hasO1morpho2 )
                    {
                        filters2->white_top_hat_o1(kernel_tophat_length2);
                    }
                    else
                    {
                        filters2->white_top_hat(kernel_tophat_length2);
                    }
                }
                else
                {
                    if( hasO1morpho2 )
                    {
                        filters2->black_top_hat_o1(kernel_tophat_length2);
                    }
                    else
                    {
                        filters2->black_top_hat(kernel_tophat_length2);
                    }
                }
            }

        }

        const unsigned char* img2_filtered;

        if( model2 == 1 )
        {
            img2_filtered = filters2->get_filtered();
        }
        if( model2 == 2 )
        {
            if( isRgb1 )
            {
                filters2->morphological_gradient_yuv(kernel_gradient_length2,alpha2,beta2,gamma2);
                img2_filtered = filters2->get_gradient();
            }
            else
            {
                filters2->morphological_gradient(kernel_gradient_length2);
                img2_filtered = filters2->get_filtered();
            }
        }

        stopTime = std::clock();
        elapsedTime = (stopTime - startTime) / double(CLOCKS_PER_SEC);
        time_filt->setText(tr("time = ")+QString::number(elapsedTime)+" s");

        if( isRgb1 )
        {


            if( model2 == 1 )
            {
                std::memcpy(image_filter_uchar,img2_filtered,3*img_size);
                std::memcpy(image_phi_uchar,img2_filtered,3*img_size);
                std::memcpy(image_shape_uchar,img2_filtered,3*img_size);
            }
            if( model2 == 2 )
            {
                unsigned char I;

                if( hasHistoNormaliz2 )
                {
                    unsigned char max = 0;
                    unsigned char min = 255;

                    for( int offset = 0; offset < img_size; offset++ )
                    {
                        if( img2_filtered[offset] > max )
                        {
                            max = img2_filtered[offset];
                        }
                        if( img2_filtered[offset] < min )
                        {
                            min = img2_filtered[offset];
                        }
                    }

                    for( int offset = 0; offset < img_size; offset++ )
                    {
                        I = (unsigned char)(255.0*double(img2_filtered[offset]-min)/double(max-min));

                        image_filter_uchar[3*offset] = I;
                        image_filter_uchar[3*offset+1] = I;
                        image_filter_uchar[3*offset+2] = I;

                        image_phi_uchar[3*offset] = I;
                        image_phi_uchar[3*offset+1] = I;
                        image_phi_uchar[3*offset+2] = I;

                        image_shape_uchar[3*offset] = I;
                        image_shape_uchar[3*offset+1] = I;
                        image_shape_uchar[3*offset+2] = I;
                    }
                }

                else
                {
                    for( int offset = 0; offset < img_size; offset++ )
                    {
                        I = img2_filtered[offset];

                        image_filter_uchar[3*offset] = I;
                        image_filter_uchar[3*offset+1] = I;
                        image_filter_uchar[3*offset+2] = I;

                        image_phi_uchar[3*offset] = I;
                        image_phi_uchar[3*offset+1] = I;
                        image_phi_uchar[3*offset+2] = I;

                        image_shape_uchar[3*offset] = I;
                        image_shape_uchar[3*offset+1] = I;
                        image_shape_uchar[3*offset+2] = I;
                    }
                }
            }


        }
        else
        {


            unsigned char I;

            if( model2 == 2 && hasHistoNormaliz2 )
            {
                unsigned char max = 0;
                unsigned char min = 255;

                for( int offset = 0; offset < img_size; offset++ )
                {
                    if( img2_filtered[offset] > max )
                    {
                        max = img2_filtered[offset];
                    }
                    if( img2_filtered[offset] < min )
                    {
                        min = img2_filtered[offset];
                    }
                }

                for( int offset = 0; offset < img_size; offset++ )
                {
                    I = (unsigned char)(255.0*double(img2_filtered[offset]-min)/double(max-min));

                    image_filter_uchar[3*offset] = I;
                    image_filter_uchar[3*offset+1] = I;
                    image_filter_uchar[3*offset+2] = I;

                    image_phi_uchar[3*offset] = I;
                    image_phi_uchar[3*offset+1] = I;
                    image_phi_uchar[3*offset+2] = I;

                    image_shape_uchar[3*offset] = I;
                    image_shape_uchar[3*offset+1] = I;
                    image_shape_uchar[3*offset+2] = I;
                }
            }

            else
            {
                for( int offset = 0; offset < img_size; offset++ )
                {
                    I = img2_filtered[offset];

                    image_filter_uchar[3*offset] = I;
                    image_filter_uchar[3*offset+1] = I;
                    image_filter_uchar[3*offset+2] = I;

                    image_phi_uchar[3*offset] = I;
                    image_phi_uchar[3*offset+1] = I;
                    image_phi_uchar[3*offset+2] = I;

                    image_shape_uchar[3*offset] = I;
                    image_shape_uchar[3*offset+1] = I;
                    image_shape_uchar[3*offset+2] = I;
                }
            }


        }

        phi_visu(false);
    }
    return;
}

// Fonction appelée lorsque on change de taille d'image dans l'onglet Display
void ImageViewer::change_display_size()
{
    if( img1 != NULL )
    {
        phi_visu(false);
    }
}

// Fonction appelée à chaque changement d'onglet
void ImageViewer::tab_visu(int value)
{
    if( img1 != NULL )
    {
        // si onglet initilisation
        if( value == 1 )
        {
            imageLabel_settings->set_doWheelEvent(false);
            // visualisation de la forme
            phi_visu(true);
            shape_visu();
        }
        else
        {
            imageLabel_settings->set_doWheelEvent(true);
            // visualisation de phi
            phi_visu(false); 
        }
    }
}

// Evénement clic souris dans imageLabel (image de la fenêtre principale) permet de rentrer dans la boucle infinie ou d'en sortir pour bloquer momentanément l'algo et prendre du temps sur une étape du contour actif
void ImageViewer::mousePressEvent(QMouseEvent *event)
{
    // si l'image est chargée
    if( img1 != NULL )
    {
        if( !startAct->isEnabled() )
        {
            if( event->button() == Qt::LeftButton )
            {
                statusBar()->showMessage(tr("Release the left mouse button to evolve the active contour."));
                hasStepByStep = false;
                hasClickStopping = true;
            }

            if( event->button() == Qt::RightButton )
            {
                hasClickStopping = false;
                hasStepByStep = true;
            }
        }
        else
        {
            if( event->button() == Qt::LeftButton )
            {
                statusBar()->showMessage(tr("Release the left mouse button to evolve the active contour."));
                hasStepByStep = false;
                hasClickStopping = true;
                emit( startAct->trigger() );
            }

            if( event->button() == Qt::RightButton )
            {
                hasStepByStep = true;
                emit( startAct->trigger() );
            }
        }
    }
}

// Evénement clic souris dans imageLabel (image de la fenêtre principale) permet de rentrer dans la boucle infinie ou d'en sortir pour bloquer momentanément l'algo et prendre du temps sur une étape du contour actif
void ImageViewer::mouseReleaseEvent(QMouseEvent *event)
{
    // si l'image est chargée
    if( img1 != NULL )
    {
        if( event->button() == Qt::LeftButton )
        {
            statusBar()->showMessage(tr("Active contour evolving."));
            if( !startAct->isEnabled() )
            {
                hasStepByStep = false;
                hasClickStopping = false;
            }
        }
    }
}

// Evénement touche clavier enfoncé dans imageLabel (image de la fenêtre principale) permet de rentrer dans la boucle infinie ou d'en sortir pour bloquer momentanément l'algo et prendre du temps sur une étape du contour actif
void ImageViewer::keyPressEvent(QKeyEvent *event)
{
    // si l'image est chargée
    if( img1 != NULL )
    {
        if( !startAct->isEnabled() )
        {
            if( !event->isAutoRepeat() )
            {
                if( event->key() == Qt::Key_Space || event->key() == Qt::Key_Right || event->key() == Qt::Key_Pause )
                {
                    hasStepByStep = false;
                    hasClickStopping = true;
                }
            }

            if( event->key() == Qt::Key_Return || event->key() == Qt::Key_Up || event->key() == Qt::Key_Down )
            {
                hasClickStopping = false;
                hasStepByStep = true;
            }
        }
        else
        {
            if( !event->isAutoRepeat() )
            {
                if( event->key() == Qt::Key_Space || event->key() == Qt::Key_Right || event->key() == Qt::Key_Pause )
                {
                    hasStepByStep = false;
                    hasClickStopping = true;
                    emit( startAct->trigger() );
                }
            }

            if( event->key() == Qt::Key_Return || event->key() == Qt::Key_Up || event->key() == Qt::Key_Down )
            {
                hasStepByStep = true;
                emit( startAct->trigger() );
            }
        }
    }
}

// Evénement touche clavier relachée dans imageLabel (image de la fenêtre principale) permet de rentrer dans la boucle infinie ou d'en sortir pour bloquer momentanément l'algo et prendre du temps sur une étape du contour actif
void ImageViewer::keyReleaseEvent(QKeyEvent *event)
{
    // si l'image est chargée
    if( img1 != NULL )
    {
        if( !event->isAutoRepeat() )
        {
            if( event->key() == Qt::Key_Space || event->key() == Qt::Key_Right || event->key() == Qt::Key_Pause )
            {
                if( !startAct->isEnabled() )
                {
                    hasStepByStep = false;
                    hasClickStopping = false;
                }
            }
        }
    }
}

// Boucle infinie dont on peut rentrer ou sortir par un clic gauche ou droite (sur imageLabel, image de la fenêtre principale) lors de l'execution de l'algorithme et permettant de voir une étape particulière ou chaque étape, au rythme voulue
void ImageViewer::infinite_loop()
{
    if( hasClickStopping )
    {
        bool temp = imageLabel->hasMouseTracking();
        imageLabel->setMouseTracking(true);

        if( hasStepByStep )
        {
            statusBar()->showMessage(tr("Click on the right mouse button to evolve the contour of one iteration."));
        }

        while( hasClickStopping )
        {
            QApplication::processEvents();
        }

        imageLabel->setMouseTracking(temp);
    }
}

// Evénement déplacement souris au niveau de imageLabel pour pouvoir donner les infos pixels
void ImageViewer::mouseMoveEvent(QMouseEvent *event)
{
    // si l'image est chargée
    if( img1 != NULL )
    {
        positionX = int(double(img_width)*double(   (  (event->pos()).x() -imageLabel->get_xoffset()    ) /double(imageLabel->getPixWidth())));
        positionY = int(double(img_height)*double(   (  (event->pos()).y() -imageLabel->get_yoffset()   ) /double(imageLabel->getPixHeight())));

        if( positionX >= 0 && positionX < img_width && positionY >= 0 && positionY < img_height )
        {
            if( image_disp == 1 )
            {
                if( !isRgb1 )
                {
                    int I = img1[ positionX+positionY*img_width ];
                    QColor RGB(I,I,I);
                    pixel_text->setText(tr("img(")+QString::number(positionX)+","+QString::number(positionY)+") = "+"<font color="+RGB.name()+">"+QString::number(I));

                }
                else
                {
                    QColor RGB(img1[ 3*(positionX+positionY*img_width) ], img1[ 3*(positionX+positionY*img_width)+1 ], img1[ 3*(positionX+positionY*img_width)+2 ]);
                    pixel_text->setText(tr("img(")+QString::number(positionX)+","+QString::number(positionY)+") = "+"<font color="+RGB.name()+">"+"("+QString::number(img1[ 3*(positionX+positionY*img_width) ])+","+QString::number(img1[ 3*(positionX+positionY*img_width)+1 ])+","+QString::number(img1[ 3*(positionX+positionY*img_width)+2 ])+")");
                }

            }

            if( image_disp == 2 )
            {
                const unsigned char* img1_filtered;

                QString img_str;
                if( hasGaussianNoise || hasSaltNoise || hasSpeckleNoise || hasMeanFilt || hasGaussianFilt || hasMedianFilt || hasAnisoDiff || hasOpenFilt || hasCloseFilt || hasTophatFilt || model == 2 )
                {
                    img_str = tr("img'(");
                }
                else
                {
                    img_str = tr("img(");
                }

                if( !isRgb1 )
                {
                    img1_filtered = filters1->get_filtered();
                    int I = img1_filtered[positionX+positionY*img_width];
                    QColor RGB(I,I,I);
                    pixel_text->setText(img_str+QString::number(positionX)+","+QString::number(positionY)+") = "+"<font color="+RGB.name()+">"+QString::number(I));
                }

                else
                {
                    if( model == 1 )
                    {
                        img1_filtered = filters1->get_filtered();

                        QColor RGB(img1_filtered[3*(positionX+positionY*img_width)],img1_filtered[3*(positionX+positionY*img_width)+1],img1_filtered[3*(positionX+positionY*img_width)+2]);

                        pixel_text->setText(img_str+QString::number(positionX)+","+QString::number(positionY)+") = "+"<font color="+RGB.name()+">"+"("+QString::number(img1_filtered[3*(positionX+positionY*img_width)])+","+QString::number(img1_filtered[3*(positionX+positionY*img_width)+1])+","+QString::number(img1_filtered[3*(positionX+positionY*img_width)+2])+")");
                    }
                    if( model == 2 )
                    {
                        img1_filtered = filters1->get_gradient();

                        int I = img1_filtered[ positionX+positionY*img_width ];
                        QColor RGB(I,I,I);
                        pixel_text->setText(img_str+QString::number(positionX)+","+QString::number(positionY)+") = "+"<font color="+RGB.name()+">"+QString::number(I));
                    }

                }
            }
            show_phi_list_value();
        }
    }

}

void ImageViewer::show_phi_list_value()
{
    QColor RGBout, RGBin;
    int Cout1, Cin1, CoutR, CoutG, CoutB, CinR, CinG, CinB;
    QColor RGBout_list(Rout1,Gout1,Bout1);
    QColor RGBin_list(Rin1,Gin1,Bin1);

    QString Cout_str = QString("<font color="+RGBout_list.name()+">"+"Cout"+"<font color=black>"+" = ");
    QString Cin_str = QString("<font color="+RGBin_list.name()+">"+"Cin"+"<font color=black>"+" = ");

    unsigned char otsu_threshold1;
    int iteration_max1;

    const char* phi = NULL;

    if( ac != NULL )
    {
        phi = ac->get_phi();

        iteration1 = ac->get_iteration();
        iteration_max1 = ac->get_iteration_max();

        if( iteration1 < iteration_max1 )
        {
            text->setText("<font color=green>"+tr("iteration t = ")+QString::number(iteration1));
        }
        else
        {
            text->setText("<font color=red>"+tr("iteration t = ")+QString::number(iteration1));
        }

        if( ac->get_hasListsChanges() )
        {
            changes_text->setText("<font color=green>"+tr("lists changes = true"));
        }
        else
        {
            changes_text->setText("<font color=red>"+tr("lists changes = false"));
        }

        if( ac->get_hasOscillation() )
        {
            oscillation_text->setText("<font color=red>"+tr("oscillation = true"));
        }
        else
        {
            oscillation_text->setText("<font color=green>"+tr("oscillation = false"));
        }


        if( model == 1 )
        {
            if( !isRgb1 )
            {
                // pour l'affichage
                if( dynamic_cast<ofeli::ACwithoutEdges*>(ac) != NULL )
                {
                    Cout1 = dynamic_cast<ofeli::ACwithoutEdges*>(ac)->get_Cout();
                    Cin1 = dynamic_cast<ofeli::ACwithoutEdges*>(ac)->get_Cin();
                    RGBout = QColor(Cout1,Cout1,Cout1);
                    RGBin = QColor(Cin1,Cin1,Cin1);
                    Cout_text->setText(Cout_str+"<font color="+RGBout.name()+">"+QString::number(Cout1));
                    Cin_text->setText(Cin_str+"<font color="+RGBin.name()+">"+QString::number(Cin1));
                }
            }
            else
            {
                if( dynamic_cast<ofeli::ACwithoutEdgesYUV*>(ac) != NULL )
                {
                    CoutR = dynamic_cast<ofeli::ACwithoutEdgesYUV*>(ac)->get_CoutR();
                    CoutG = dynamic_cast<ofeli::ACwithoutEdgesYUV*>(ac)->get_CoutG();
                    CoutB = dynamic_cast<ofeli::ACwithoutEdgesYUV*>(ac)->get_CoutB();

                    CinR = dynamic_cast<ofeli::ACwithoutEdgesYUV*>(ac)->get_CinR();
                    CinG = dynamic_cast<ofeli::ACwithoutEdgesYUV*>(ac)->get_CinG();
                    CinB = dynamic_cast<ofeli::ACwithoutEdgesYUV*>(ac)->get_CinB();

                    RGBout = QColor(CoutR,CoutG,CoutB);
                    RGBin = QColor(CinR,CinG,CinB);

                    Cout_text->setText(Cout_str+"<font color="+RGBout.name()+">"+"("+QString::number(CoutR)+","+QString::number(CoutG)+","+QString::number(CoutB)+")");
                    Cin_text->setText(Cin_str+"<font color="+RGBin.name()+">"+"("+QString::number(CinR)+","+QString::number(CinB)+","+QString::number(CinB)+")");
                }
            }
        }

        if( model == 2 )
        {
            if( dynamic_cast<ofeli::GeodesicAC*>(ac) != NULL )
            {
                // pour l'affichage
                otsu_threshold1 = dynamic_cast<ofeli::GeodesicAC*>(ac)->get_otsu_threshold();
                threshold_text->setText(tr("Otsu's threshold of gradient = ")+QString::number(otsu_threshold1));
            }
        }
    }

    if( phi != NULL )
    {
        QColor RGBout(Rout1,Gout1,Bout1);
        QColor RGBin(Rin1,Gin1,Bin1);

        if( phi[ positionX+positionY*img_width ] == 3 )
        {
            phi_text->setText("ϕ(x,y,t) = <font color="+RGBout.name()+">"+QString::number(phi[positionX+positionY*img_width]));
            lists_text->setText("(x,y) ∉ <font color="+RGBout.name()+">"+"Lout"+"<font color=black>"+"(t) & ∉ "+"<font color="+RGBin.name()+">"+"Lin"+"<font color=black>"+"(t)");
        }
        if( phi[ positionX+positionY*img_width ] == -3 )
        {
            phi_text->setText("ϕ(x,y,t) = <font color="+RGBin.name()+">"+QString::number(phi[positionX+positionY*img_width]));
            lists_text->setText("(x,y) ∉ <font color="+RGBin.name()+">"+"Lin"+"<font color=black>"+"(t) & ∉ "+"<font color="+RGBout.name()+">"+"Lout"+"<font color=black>"+"(t)");
        }
        if( phi[ positionX+positionY*img_width ] == 1 )
        {
            phi_text->setText("ϕ(x,y,t) = <font color="+RGBout.name()+">"+QString::number(phi[positionX+positionY*img_width]));
            lists_text->setText("(x,y) ∈ <font color="+RGBout.name()+">"+"Lout"+"<font color=black>"+"(t)");
        }
        if( phi[ positionX+positionY*img_width ] == -1 )
        {
            phi_text->setText("ϕ(x,y,t) = <font color="+RGBin.name()+">"+QString::number(phi[positionX+positionY*img_width]));
            lists_text->setText("(x,y) ∈ <font color="+RGBin.name()+">"+"Lin"+"<font color=black>"+"(t)");
        }
    }
}

// Pour arrêter manuellement l'algorithme, s'il y a un problème au niveau des conditions d'arrêts ou du temps de calcul sur les grandes images
void ImageViewer::stop()
{
    hasClickStopping = false;
    hasAlgoBreaking = true;
}

// Filtre des événements pour avoir le tracking au niveau du widget image de la fenêtre principale et de la fenêtre de parametres et pour ne pas avoir le tracking/la position au niveau de l'ensemble de chaque fenêtre
bool ImageViewer::eventFilter(QObject *object, QEvent *event)
{
    // deplacement uniquement en fonction de imageLabel et pas main_window
    if( object == imageLabel && event->type() == QEvent::MouseMove )
    {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        mouseMoveEvent(mouseEvent);
    }

    // molette prise en compte que sur la zone de imageLabel
    if( object == imageLabel && event->type() == QEvent::Wheel && img1 != NULL )
    {
        imageLabel->set_hasText(false);
        imageLabel->setBackgroundRole(QPalette::Dark);
        disconnect(scale_spin0,SIGNAL(valueChanged(int)),this,SLOT(do_scale0(int)));
        disconnect(scale_slider0,SIGNAL(valueChanged(int)),scale_spin0,SLOT(setValue(int)));
        disconnect(scale_spin0,SIGNAL(valueChanged(int)),scale_slider0,SLOT(setValue(int)));
        //disconnect(scrollArea->verticalScrollBar(),SIGNAL(rangeChanged(int,int)),this,SLOT(adjustVerticalScroll (int,int)));
        //disconnect(scrollArea->horizontalScrollBar(),SIGNAL(rangeChanged(int,int)),this,SLOT(adjustHorizontalScroll(int,int)));
        disconnect(scale_spin,SIGNAL(valueChanged(int)),this,SLOT(do_scale(int)));
        disconnect(scale_slider,SIGNAL(valueChanged(int)),scale_spin,SLOT(setValue(int)));
        disconnect(scale_spin,SIGNAL(valueChanged(int)),scale_slider,SLOT(setValue(int)));

        QApplication::processEvents();

        /*scale_spin0->setValue(int(100.0f*imageLabel->get_zoomFactor()));
        scale_slider0->setValue(int(100.0f*imageLabel->get_zoomFactor()));
        scale_spin->setValue(int(100.0f*imageLabel->get_zoomFactor()));
        scale_slider->setValue(int(100.0f*imageLabel->get_zoomFactor()));*/

        scale_spin0->setValue(100.0f*imageLabel->get_zoomFactor()/img_width);
        scale_slider0->setValue(100.0f*imageLabel->get_zoomFactor()/img_width);
        scale_spin->setValue(100.0f*imageLabel->get_zoomFactor()/img_width);
        scale_slider->setValue(100.0f*imageLabel->get_zoomFactor()/img_width);

        connect(scale_spin0,SIGNAL(valueChanged(int)),this,SLOT(do_scale0(int)));
        connect(scale_slider0,SIGNAL(valueChanged(int)),scale_spin0,SLOT(setValue(int)));
        connect(scale_spin0,SIGNAL(valueChanged(int)),scale_slider0,SLOT(setValue(int)));

        connect(scale_spin,SIGNAL(valueChanged(int)),this,SLOT(do_scale(int)));
        connect(scale_slider,SIGNAL(valueChanged(int)),scale_spin,SLOT(setValue(int)));
        connect(scale_spin,SIGNAL(valueChanged(int)),scale_slider,SLOT(setValue(int)));
    }

    if( (object == scale_spin0 || object == scale_slider0 || object == scale_spin || object == scale_slider) && event->type() == QEvent::MouseButtonPress )
    {
        if( img1 != NULL )
        {
            imageLabel->set_hasText(false);
            imageLabel->setBackgroundRole(QPalette::Dark);
            imageLabel_settings->set_hasText(false);
            imageLabel_settings->setBackgroundRole(QPalette::Dark);
        }
        positionX = img_width/2;
        positionY = img_height/2;
    }


    // deplacement uniquement en fonction de imageLabel_settings et pas settings_window
    if( object == imageLabel_settings && event->type() == QEvent::MouseMove )
    {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        mouseMoveEvent2(mouseEvent);
    }

    // clic pris en compte que sur la zone de imageLabel_settings
    if( object == imageLabel_settings && event->type() == QEvent::MouseButtonPress )
    {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        mousePressEvent2(mouseEvent);
    }

    // molette prise en compte que sur la zone de imageLabel_settings
    if( object == imageLabel_settings && event->type() == QEvent::Wheel && img1 != NULL )
    {
        imageLabel_settings->set_hasText(false);
        imageLabel_settings->setBackgroundRole(QPalette::Dark);
        disconnect(scrollArea_settings->verticalScrollBar(),SIGNAL(rangeChanged(int,int)),this,SLOT(adjustVerticalScroll_settings(int,int)));
        disconnect(scrollArea_settings->horizontalScrollBar(),SIGNAL(rangeChanged(int,int)),this,SLOT(adjustHorizontalScroll_settings(int,int)));
        scale_spin->setValue(int(100.0f*imageLabel_settings->get_zoomFactor()));
        connect(scrollArea_settings->verticalScrollBar(),SIGNAL(rangeChanged(int,int)),this,SLOT(adjustVerticalScroll_settings(int,int)));
        connect(scrollArea_settings->horizontalScrollBar(),SIGNAL(rangeChanged(int,int)),this,SLOT(adjustHorizontalScroll_settings(int,int)));
        QWheelEvent* mouseEvent = static_cast<QWheelEvent*>(event);
        wheelEvent(mouseEvent);
    }

    if( object == scale_spinA && event->type() == QEvent::MouseButtonPress )
    {
        if( !image1.isNull() )
        {
            imageLabelA->set_hasText(false);
            imageLabelA->setBackgroundRole(QPalette::Dark);
        }
        positionXa = imgA_width/2;
        positionYa = imgA_height/2;
    }

    if( object == scale_spinB && event->type() == QEvent::MouseButtonPress )
    {
        if( !image2.isNull() )
        {
            imageLabelB->set_hasText(false);
            imageLabelB->setBackgroundRole(QPalette::Dark);
        }
        positionXb = imgB_width/2;
        positionYb = imgB_height/2;
    }

    // deplacement uniquement en fonction de imageLabelA
    if( object == imageLabelA && event->type() == QEvent::MouseMove )
    {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        mouseMoveEventA(mouseEvent);
    }
    // clic pris en compte que sur la zone de imageLabelA
    if( object == imageLabelA && event->type() == QEvent::MouseButtonPress )
    {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        mousePressEventA(mouseEvent);
    }

    if( object == imageLabelA && event->type() == QEvent::Wheel && !image1.isNull() )
    {
        imageLabelA->set_hasText(false);
        imageLabelA->setBackgroundRole(QPalette::Dark);
        disconnect(scrollAreaA->verticalScrollBar(),SIGNAL(rangeChanged(int,int)),this,SLOT(adjustVerticalScrollA (int,int)));
        disconnect(scrollAreaA->horizontalScrollBar(),SIGNAL(rangeChanged(int,int)),this,SLOT(adjustHorizontalScrollA(int,int)));
        scale_spinA->setValue(int(100.0f*imageLabelA->get_zoomFactor()));
        connect(scrollAreaA->verticalScrollBar(),SIGNAL(rangeChanged(int,int)),this,SLOT(adjustVerticalScrollA (int,int)));
        connect(scrollAreaA->horizontalScrollBar(),SIGNAL(rangeChanged(int,int)),this,SLOT(adjustHorizontalScrollA(int,int)));
    }

    if( object == imageLabelB && event->type() == QEvent::Wheel && !image2.isNull() )
    {
        imageLabelB->set_hasText(false);
        imageLabelB->setBackgroundRole(QPalette::Dark);
        disconnect(scrollAreaB->verticalScrollBar(),SIGNAL(rangeChanged(int,int)),this,SLOT(adjustVerticalScrollB (int,int)));
        disconnect(scrollAreaB->horizontalScrollBar(),SIGNAL(rangeChanged(int,int)),this,SLOT(adjustHorizontalScrollB(int,int)));
        scale_spinB->setValue(int(100.0f*imageLabelB->get_zoomFactor()));
        connect(scrollAreaB->verticalScrollBar(),SIGNAL(rangeChanged(int,int)),this,SLOT(adjustVerticalScrollB (int,int)));
        connect(scrollAreaB->horizontalScrollBar(),SIGNAL(rangeChanged(int,int)),this,SLOT(adjustHorizontalScrollB(int,int)));
    }

    // deplacement uniquement en fonction de imageLabelB
    if( object == imageLabelB && event->type() == QEvent::MouseMove )
    {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        mouseMoveEventB(mouseEvent);
    }
    // clic pris en compte que sur la zone de imageLabelB
    if( object == imageLabelB && event->type() == QEvent::MouseButtonPress )
    {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        mousePressEventB(mouseEvent);
    }


    if( object == imageLabel && event->type() == QEvent::DragEnter )
    {
        QDragEnterEvent* drag = static_cast<QDragEnterEvent*>(event);
        dragEnterEvent(drag);
    }
    if( object == imageLabel && event->type() == QEvent::DragMove )
    {
        QDragMoveEvent* drag = static_cast<QDragMoveEvent*>(event);
        dragMoveEvent(drag);
    }
    if( object == imageLabel && event->type() == QEvent::Drop )
    {
        QDropEvent* drag = static_cast<QDropEvent*>(event);
        dropEvent(drag);
    }
    if( object == imageLabel && event->type() == QEvent::DragLeave )
    {
        QDragLeaveEvent* drag = static_cast<QDragLeaveEvent*>(event);
        dragLeaveEvent(drag);
    }

    if( object == imageLabel_settings && event->type() == QEvent::DragEnter )
    {
        QDragEnterEvent* drag = static_cast<QDragEnterEvent*>(event);
        dragEnterEventPhi(drag);
    }
    if( object == imageLabel_settings && event->type() == QEvent::DragMove )
    {
        QDragMoveEvent* drag = static_cast<QDragMoveEvent*>(event);
        dragMoveEventPhi(drag);
    }
    if( object == imageLabel_settings && event->type() == QEvent::Drop )
    {
        QDropEvent* drag = static_cast<QDropEvent*>(event);
        dropEventPhi(drag);
    }
    if( object == imageLabel_settings && event->type() == QEvent::DragLeave )
    {
        QDragLeaveEvent* drag = static_cast<QDragLeaveEvent*>(event);
        dragLeaveEventPhi(drag);
    }

    if( object == imageLabelA && event->type() == QEvent::DragEnter )
    {
        QDragEnterEvent* drag = static_cast<QDragEnterEvent*>(event);
        dragEnterEventA(drag);
    }
    if( object == imageLabelA && event->type() == QEvent::DragMove )
    {
        QDragMoveEvent* drag = static_cast<QDragMoveEvent*>(event);
        dragMoveEventA(drag);
    }
    if( object == imageLabelA && event->type() == QEvent::Drop )
    {
        QDropEvent* drag = static_cast<QDropEvent*>(event);
        dropEventA(drag);
    }
    if( object == imageLabelA && event->type() == QEvent::DragLeave )
    {
        QDragLeaveEvent* drag = static_cast<QDragLeaveEvent*>(event);
        dragLeaveEventA(drag);
    }

    if( object == imageLabelB && event->type() == QEvent::DragEnter )
    {
        QDragEnterEvent* drag = static_cast<QDragEnterEvent*>(event);
        dragEnterEventB(drag);
    }
    if( object == imageLabelB && event->type() == QEvent::DragMove )
    {
        QDragMoveEvent* drag = static_cast<QDragMoveEvent*>(event);
        dragMoveEventB(drag);
    }
    if( object == imageLabelB && event->type() == QEvent::Drop )
    {
        QDropEvent* drag = static_cast<QDropEvent*>(event);
        dropEventB(drag);
    }
    if( object == imageLabelB && event->type() == QEvent::DragLeave )
    {
        QDragLeaveEvent* drag = static_cast<QDragLeaveEvent*>(event);
        dragLeaveEventB(drag);
    }

    if( object == imageLabel && event->type() == QEvent::MouseMove && positionX >= 0 && positionX < img_width && positionY >= 0 && positionY < img_height )
    {
        setCursor(Qt::CrossCursor);
    }
    else
    {
        setCursor(Qt::ArrowCursor);
    }
    return false;
}

// Evénement de déplacement de la souris dans le widget image de la fenêtre de paramètres
void ImageViewer::mouseMoveEvent2(QMouseEvent *event2)
{
    // si l'image est chargée et si on est dans l'onglet initialisation
    if( img1 != NULL )
    {
        // position de la souris dans l'image
        positionX = int(double(img_width)*double(   (  (event2->pos()).x() -imageLabel_settings->get_xoffset()   ) /double(imageLabel_settings->getPixWidth())));
        positionY = int(double(img_height)*double(   (  (event2->pos()).y() -imageLabel_settings->get_yoffset()   ) /double(imageLabel_settings->getPixHeight())));

        if( onglets->currentIndex() == 1 )
        {
            // on en deduit les valeurs relatives des sliders en %

            double a = (double(positionX)-double(img_width/2))/double(img_width);
            double b = (double(positionY)-double(img_height/2))/double(img_height);

            abscissa_spin->setValue(int(a*100.0));
            ordinate_spin->setValue(int(b*100.0));
        }
    }

}



// Evénement clic souris dans le widget image de la fenêtre de paramètres
void ImageViewer::mousePressEvent2(QMouseEvent *event2)
{
    // si on est dans l'onglet d'initilisation
    if( (img1 != NULL) && onglets->currentIndex() == 1 )
    {
        // clic gauche, on ajoute une forme
        if( event2->button() == Qt::LeftButton )
        {
            add_visu();
        }
        // clic droit, on soustraie
        if( event2->button() == Qt::RightButton )
        {
            subtract_visu();
        }
        // clic du milieu, on change un rectangle en ellipse ou vice versa
        if( event2->button() == Qt::MidButton )
        {
            if( !rectangle_radio->isChecked() )
            {
                rectangle_radio->setChecked(true);
            }
            else
            {
                ellipse_radio->setChecked(true);
            }
            shape_visu();
        }
    }

    // pour cacher ou non au clic gauche le contour actif initial quand on est dans l'onglet Preprocessing
    if( (img1 != NULL) && onglets->currentIndex() == 2 )
    {
        if( event2->button() == Qt::RightButton )
        {
            if( hasContoursHidden )
            {
                hasContoursHidden = false;
            }
            else
            {
                hasContoursHidden = true;
            }
            phi_visu(false);
        }
        if( event2->button() == Qt::LeftButton )
        {
            hasContoursHidden = true;
            if( hasShowImg1 )
            {
                hasShowImg1 = false;
                img1_visu();
            }
            else
            {
                hasShowImg1 = true;
                phi_visu(false);
            }
        }
    }

 }

// Fonction pour afficher l'image de départ par clic gauche dans l'onglet preprocessing
void ImageViewer::img1_visu()
{
    if( img1 != NULL )
    {
        imageLabel_settings->set_qimage(img);
    }
}

// Evénement de la molette de souris utilisé dans l'onglet Initialization pour agrandir ou rétrecir la forme
void ImageViewer::wheelEvent(QWheelEvent *event)
{
    // si une image est déja chargée et si on est dans l'onglet d'initialisation
    if( (img1 != NULL) && onglets->currentIndex() == 1 )
    {
        // nombre de degré et de pas de la molette
        int numDegrees = event->delta() / 8;
        int numSteps = numDegrees / 15;

        // récupération des valeurs des sliders concernant la taille de la forme
        int width_ratio = width_shape_spin->value();
        int height_ratio = height_shape_spin->value();

        // correpondance par rapport à la taille réelle de la forme
        double width = double(img_width)*double(width_ratio)/100.0;
        double height = double(img_height)*double(height_ratio)/100.0;

        // augmentation ou diminution de 15% à chaque pas de molette
        width += 0.15*double(numSteps)*width;
        height += 0.15*double(numSteps)*height;

        width_ratio = int( 100.0*width/double(img_width) );
        height_ratio = int( 100.0*height/double(img_height) );

        if( (width_ratio > 3) && (height_ratio > 3) && (width_ratio < 150) && (height_ratio < 150) )
        {
            width_shape_spin->setValue(width_ratio);
            height_shape_spin->setValue(height_ratio);
        }

        event->accept();
    }
}


///////////////////////////////////////
//                                   //
//  Fonctions liées a l'évaluation   //
//                                   //
///////////////////////////////////////


// Affiche la fenêtre d'évaluation quand on clique sur Evaluation
void ImageViewer::evaluation()
{
    // si la fenêtre d'évaluation n'est pas visible déjà
    if( !evaluation_window->isVisible() )
    {
        // on l'affiche
        evaluation_window->show();

    }
}


void ImageViewer::openFileNameA()
{
    fileNameA = QFileDialog::getOpenFileName(this,
                                            tr("Open File A"),
                                            last_directory_used,
                                            tr("Image Files (%1)").arg(nameFilters.join(" ")));
    open_imageA();
}

// Ouverture de l'image 1 appelée par le boutton 1 de la fenêtre d'évaluation
void ImageViewer::open_imageA()
{
    if( !fileNameA.isEmpty() )
    {
        image1 = QImage(fileNameA);
        if( image1.isNull() )
        {
            QMessageBox::information(this, tr("Opening error - Ofeli"),
                                     tr("Cannot load %1.").arg(fileNameA));
            return;
        }

        imageLabelA->set_qimage0(image1);
        QApplication::processEvents();

        imgA_height = image1.height();
        imgA_width = image1.width();
        imgA_size = imgA_width*imgA_height;

        if( listA != NULL )
        {
            delete[] listA;
            listA = NULL;
        }
        if( listA == NULL )
        {
            listA = new int[imgA_size];
        }

        create_listA();

        positionXa = imgA_width/2;
        positionYa = imgA_height/2;

        QFileInfo fi1(fileNameA);
        QString name1 = fi1.fileName();

        QString string_lists_text;
        string_lists_text=QString::number(imgA_width)+"×"+QString::number(imgA_height);
        label_name1->setText(name1 +" - "+string_lists_text);
    }
}

void ImageViewer::create_listA()
{
    if( listA != NULL )
    {
        unsigned char redA, greenA, blueA;

        if( Acolor_combobox->currentIndex() == 8 )
        {
            redA = redAselected;
            greenA = greenAselected;
            blueA = blueAselected;
        }
        else
        {
            get_color(Acolor_combobox->currentIndex(),redA,greenA,blueA);
        }

        // on remplit les listes
        int x, y;
        QRgb pix;
        int index = 0;

        for( int offset = 0; offset < imgA_size; offset++ )
        {
            y = offset/imgA_width;
            x = offset-y*imgA_width;

            pix = image1.pixel(x,y);

            if( (unsigned char)(qRed(pix)) == redA && (unsigned char)(qGreen(pix)) == greenA && (unsigned char)(qBlue(pix) == blueA) )
            {
                listA[index] = offset;
                index++;
            }
        }
        listA_length = index;

        if( listA_length != 0 )
        {
            text_listA_length->setText("<font color=green>"+tr("List A length = ")+QString::number(listA_length));
        }
        else
        {
            text_listA_length->setText("<font color=red>"+tr("List A length = ")+QString::number(listA_length));
        }

        if( listA_length != 0 && listB_length != 0 && listB != NULL )
        {
            onglets_eval->setTabEnabled(1,true);
        }
        else
        {
            onglets_eval->setTabEnabled(1,false);
        }
    }

    return;
}

void ImageViewer::create_listB()
{
    if( listB != NULL )
    {
        unsigned char redB, greenB, blueB;

        if( Bcolor_combobox->currentIndex() == 8 )
        {
            redB = redBselected;
            greenB = greenBselected;
            blueB = blueBselected;
        }
        else
        {
            get_color(Bcolor_combobox->currentIndex(),redB,greenB,blueB);
        }

        // on remplit les listes
        int x, y;
        QRgb pix;
        int index = 0;

        for( int offset = 0; offset < imgB_size; offset++ )
        {
            y = offset/imgB_width;
            x = offset-y*imgB_width;

            pix = image2.pixel(x,y);

            if( (unsigned char)(qRed(pix)) == redB && (unsigned char)(qGreen(pix)) == greenB && (unsigned char)(qBlue(pix) == blueB) )
            {
                listB[index] = offset;
                index++;
            }
        }
        listB_length = index;

        if( listB_length != 0 )
        {
            text_listB_length->setText("<font color=green>"+tr("List B length = ")+QString::number(listB_length));
        }
        else
        {
            text_listB_length->setText("<font color=red>"+tr("List B length = ")+QString::number(listB_length));
        }

        if( listA_length != 0 && listB_length != 0 && listA != NULL )
        {
            onglets_eval->setTabEnabled(1,true);
        }
        else
        {
            onglets_eval->setTabEnabled(1,false);
        }
    }

    return;
}

void ImageViewer::create_listA(int index)
{
    create_listA();
}

void ImageViewer::create_listB(int index)
{
    create_listB();
}

void ImageViewer::openFileNameB()
{
    fileNameB = QFileDialog::getOpenFileName(this,
                                            tr("Open File B"),
                                            last_directory_used,
                                            tr("Image Files (%1)").arg(nameFilters.join(" ")));
    open_imageB();
}

// Ouverture de l'image 2 appelée par le boutton 2 de la fenêtre d'évaluation
void ImageViewer::open_imageB()
{
    if( !fileNameB.isEmpty() )
    {
        image2 = QImage(fileNameB);
        if( image2.isNull() )
        {
            QMessageBox::information(this, tr("Opening error - Ofeli"),
                                     tr("Cannot load %1.").arg(fileNameB));
            return;
        }

        imageLabelB->set_qimage0(image2);
        QApplication::processEvents();

        imgB_height = image2.height();
        imgB_width = image2.width();
        imgB_size = imgB_width*imgB_height;

        if( listB != NULL )
        {
            delete[] listB;
            listB = NULL;
        }

        if( listB == NULL )
        {
            listB = new int[imgB_size];
        }

        create_listB();

        positionXb = imgB_width/2;
        positionYb = imgB_height/2;

        QFileInfo fi2(fileNameB);
        QString name2 = fi2.fileName();

        QString string_lists_text;
        string_lists_text=QString::number(imgB_width)+"×"+QString::number(imgB_height);
        label_name2->setText(name2 +" - "+string_lists_text);
    }
}

// Fonction appelée par le boutton select, sélection et affichage d'une couleure particulière
void ImageViewer::colorA()
{
    // Selection d'une QColor à partir d'une boîte de dialogue couleur
    QColor Acolor = QColorDialog::getColor(Qt::white, this, tr("Select list A color"));
    if( Acolor.isValid() )
    {
        redAselected = (unsigned char)(Acolor.red());
        greenAselected = (unsigned char)(Acolor.green());
        blueAselected = (unsigned char)(Acolor.blue());

        QPixmap pm(12,12);
        pm.fill(Acolor);
        Acolor_combobox->setItemIcon(8,pm);

        Acolor_combobox->setCurrentIndex(8);

        create_listA();
    }
}

// Fonction appelée par le boutton select, sélection et affichage d'une couleur particulière
void ImageViewer::colorB()
{
    // Selection d'une QColor à partir d'une boîte de dialogue couleur
    QColor Bcolor = QColorDialog::getColor(Qt::white, this, tr("Select list B color"));
    if( Bcolor.isValid() )
    {
        redBselected = (unsigned char)(Bcolor.red());
        greenBselected = (unsigned char)(Bcolor.green());
        blueBselected = (unsigned char)(Bcolor.blue());

        QPixmap pm(12,12);
        pm.fill(Bcolor);
        Bcolor_combobox->setItemIcon(8,pm);

        Bcolor_combobox->setCurrentIndex(8);

        create_listB();
    }
}

// Pour avoir la position sur l'image A pour obtenir la couleur
void ImageViewer::mouseMoveEventA(QMouseEvent *event)
{
    // si l'image est chargée
    if( !image1.isNull() )
    {
        // position de la souris dans l'image
        positionXa = int(double(imgA_width)*double(   (  (event->pos()).x() -imageLabelA->get_xoffset()   ) /double(imageLabelA->getPixWidth())));
        positionYa = int(double(imgA_height)*double(   (  (event->pos()).y() -imageLabelA->get_yoffset()   ) /double(imageLabelA->getPixHeight())));
    }
}

// Pour avoir la position sur l'image B pour obtenir la couleur
void ImageViewer::mouseMoveEventB(QMouseEvent *event)
{
    // si l'image est chargée
    if( !image2.isNull() )
    {
        // position de la souris dans l'image
        positionXb = int(double(imgB_width)*double(   (  (event->pos()).x() -imageLabelB->get_xoffset()   ) /double(imageLabelB->getPixWidth())));
        positionYb = int(double(imgB_height)*double(   (  (event->pos()).y() -imageLabelB->get_yoffset()   ) /double(imageLabelB->getPixHeight())));

    }
}

// Pour sélectionner la couleur de la liste A en cliquant sur l'image A
void ImageViewer::mousePressEventA(QMouseEvent *event)
{
    if( !image1.isNull() )
    {
        QRgb pix = image1.pixel(positionXa,positionYa);

        redAselected = (unsigned char)(qRed(pix));
        greenAselected = (unsigned char)(qGreen(pix));
        blueAselected = (unsigned char)(qBlue(pix));

        QPixmap pm(12,12);
        pm.fill(QColor(redAselected, greenAselected, blueAselected));
        Acolor_combobox->setItemIcon(8,pm);

        Acolor_combobox->setCurrentIndex(8);

        create_listA();
    }

}

// Pour sélectionner la couleur de la liste B en cliquant sur l'image B
void ImageViewer::mousePressEventB(QMouseEvent *event)
{
    if( !image2.isNull() )
    {
        QRgb pix = image2.pixel(positionXb,positionYb);

        redBselected = (unsigned char)(qRed(pix));
        greenBselected = (unsigned char)(qGreen(pix));
        blueBselected = (unsigned char)(qBlue(pix));

        QPixmap pm(12,12);
        pm.fill(QColor(redBselected, greenBselected, blueBselected));
        Bcolor_combobox->setItemIcon(8,pm);

        Bcolor_combobox->setCurrentIndex(8);

        create_listB();
    }
}

// Fonction exécutée quand on appuye sur l'onglet result de la fenêtre d'évaluation
// calcule et affiche les distances d'Hausdorff et Hausdorff modifiée
void ImageViewer::compute(int index)
{
    if( index == 1 )
    {
        // création d'un objet a partir des listes
        ofeli::HausdorffDistance hausdorffDistance1(listA, listA_length, imgA_width, imgA_height, listB, listB_length, imgB_width, imgB_height);

        // utilisation des accesseurs
        double hausdorff_dist1 = hausdorffDistance1.get_hausdorff_dist();
        double modified_hausdorff_dist1 = hausdorffDistance1.get_modified_hausdorff_dist();
        double hausdorff_ratio1 = hausdorffDistance1.get_hausdorff_ratio();
        double modified_hausdorff_ratio1 = hausdorffDistance1.get_modified_hausdorff_ratio();
        double centroids_dist1 = hausdorffDistance1.get_centroids_dist();
        double centroids_ratio1 = hausdorffDistance1.get_centroids_ratio();

        // affichage
        label_hausdorff->setText(tr("Hausdorff distance = ")+QString::number(hausdorff_dist1)+(tr(" pixels")));
        label_hausdorff_ratio->setText(tr("Hausdorff ratio = ")+QString::number(100.0*hausdorff_ratio1)+(" %"));
        label_modified_hausdorff->setText(tr("modified Hausdorff distance = ")+QString::number(modified_hausdorff_dist1)+(tr(" pixels")));
        label_modified_hausdorff_ratio->setText(tr("modified Hausdorff ratio = ")+QString::number(100.0*modified_hausdorff_ratio1)+(" %"));
        label_centroids_dist->setText(tr("distance between centroids = ")+QString::number(centroids_dist1)+(tr(" pixels")));
        label_centroids_ratio->setText(tr("ratio between centroids = ")+QString::number(100.0*centroids_ratio1)+(" %"));
    }
}

// Fonction appelée pour le changement d'échelle dans la fenêtre viewer
void ImageViewer::do_scale0(int value)
{
    if( img1 != NULL )
    {
        double scale_factor = value/100.0;
        imageLabel->setZoomFactor(scale_factor);
    }
}

void ImageViewer::scaleA(int value)
{
    if( !image1.isNull() )
    {
        double scale_factor = value/100.0;
        imageLabelA->setZoomFactor(scale_factor);
    }
}

void ImageViewer::scaleB(int value)
{
    if( !image2.isNull() )
    {
        double scale_factor = value/100.0;
        imageLabelB->setZoomFactor(scale_factor);
    }

}

void ImageViewer::dragEnterEvent(QDragEnterEvent *event)
{
    if( !settings_window->isVisible() )
    {
        QString text(tr("<drop image>"));
        imageLabel->set_text(text);
        imageLabel->setBackgroundRole(QPalette::Highlight);
        imageLabel->set_hasText(true);

        event->acceptProposedAction();
        emit changed(event->mimeData());
    }
}

void ImageViewer::dragMoveEvent(QDragMoveEvent *event)
{
    if( !settings_window->isVisible() )
    {
        event->acceptProposedAction();
    }
}

void ImageViewer::dropEvent(QDropEvent *event)
{
    if( !settings_window->isVisible() )
    {
        const QMimeData* mimeData = event->mimeData();

        if( mimeData->hasUrls() )
        {
            QList<QUrl> urlList = mimeData->urls();
            fileName = urlList.first().toLocalFile();
        }
        imageLabel->setBackgroundRole(QPalette::Dark);
        open();
        event->acceptProposedAction();
    }
}

void ImageViewer::dragLeaveEvent(QDragLeaveEvent *event)
{
    if( !settings_window->isVisible() )
    {
        QString text(tr("<drag image>"));
        imageLabel->set_text(text);
        imageLabel->setBackgroundRole(QPalette::Dark);
        imageLabel->set_hasText(true);
        emit changed();
        event->accept();
    }
}



void ImageViewer::dragEnterEventPhi(QDragEnterEvent *event)
{
    if( onglets->currentIndex() != 1 )
    {
        onglets->setCurrentIndex(1);
    }
    QString text(tr("<drop ϕ(t=0)>"));
    imageLabel_settings->set_text(text);
    imageLabel_settings->setBackgroundRole(QPalette::Highlight);
    imageLabel_settings->set_hasText(true);

    event->acceptProposedAction();
    emit changed(event->mimeData());
}

void ImageViewer::dragMoveEventPhi(QDragMoveEvent *event)
{
    event->acceptProposedAction();
}

void ImageViewer::dropEventPhi(QDropEvent *event)
{
    const QMimeData* mimeData = event->mimeData();

    if( mimeData->hasUrls() )
    {
        QList<QUrl> urlList = mimeData->urls();
        fileName_phi = urlList.first().toLocalFile();
    }
    imageLabel_settings->setBackgroundRole(QPalette::Dark);
    open_phi();
    event->acceptProposedAction();
}

void ImageViewer::dragLeaveEventPhi(QDragLeaveEvent *event)
{
    QString text(tr("<drag ϕ(t=0)>"));
    imageLabel_settings->set_text(text);
    imageLabel_settings->setBackgroundRole(QPalette::Dark);
    imageLabel_settings->set_hasText(true);

    emit changed();
    event->accept();
}

void ImageViewer::dragEnterEventA(QDragEnterEvent *event)
{
    QString text(tr("<drop image>"));
    imageLabelA->set_text(text);
    imageLabelA->setBackgroundRole(QPalette::Highlight);
    imageLabelA->set_hasText(true);

    event->acceptProposedAction();
    emit changed(event->mimeData());
}

void ImageViewer::dragMoveEventA(QDragMoveEvent *event)
{
    event->acceptProposedAction();
}

void ImageViewer::dropEventA(QDropEvent *event)
{
    const QMimeData* mimeData = event->mimeData();

    if( mimeData->hasUrls() )
    {
        QList<QUrl> urlList = mimeData->urls();
        fileNameA = urlList.first().toLocalFile();
    }
    imageLabelA->setBackgroundRole(QPalette::Dark);
    open_imageA();
    event->acceptProposedAction();
}

void ImageViewer::dragLeaveEventA(QDragLeaveEvent *event)
{
    QString text(tr("<drag image>"));
    imageLabelA->set_text(text);
    imageLabelA->setBackgroundRole(QPalette::Dark);
    imageLabelA->set_hasText(true);
    emit changed();
    event->accept();
}


void ImageViewer::dragEnterEventB(QDragEnterEvent *event)
{
    QString text(tr("<drop image>"));
    imageLabelB->set_text(text);
    imageLabelB->setBackgroundRole(QPalette::Highlight);
    imageLabelB->set_hasText(true);

    event->acceptProposedAction();
    emit changed(event->mimeData());
}

void ImageViewer::dragMoveEventB(QDragMoveEvent *event)
{
    event->acceptProposedAction();
}

void ImageViewer::dropEventB(QDropEvent *event)
{
    const QMimeData* mimeData = event->mimeData();

    if( mimeData->hasUrls() )
    {
        QList<QUrl> urlList = mimeData->urls();
        fileNameB = urlList.first().toLocalFile();
    }
    imageLabelB->setBackgroundRole(QPalette::Dark);
    open_imageB();
    event->acceptProposedAction();
}

void ImageViewer::dragLeaveEventB(QDragLeaveEvent *event)
{
    QString text(tr("<drag image>"));
    imageLabelB->set_text(text);
    imageLabelB->setBackgroundRole(QPalette::Dark);
    imageLabelB->set_hasText(true);
    emit changed();
    event->accept();
}

void ImageViewer::closeEvent(QCloseEvent *event)
{
    QSettings settings("Bessy", "Ofeli");

    settings.setValue("mainWindow/size", size());
    settings.setValue("mainWindow/position", pos());

    settings.setValue("settingsWindow/size", settings_window->size());
    settings.setValue("settingsWindow/position", settings_window->pos());

    settings.setValue("evaluationWindow/size", evaluation_window->size());
    settings.setValue("evalutationWindow/position", evaluation_window->pos());

    settings.setValue("aboutWindow/position", about_window->pos());

    settings.setValue("licenseWindow/size", license_window->size());
    settings.setValue("licenseWindow/position", license_window->pos());


    settings.setValue("name/last_directory_used", last_directory_used);
    settings.setValue("Settings/Initialization/img_phi", img_phi);
    settings.setValue("Settings/Display/zoom_factor", scale_spin0->value());

    settings.setValue("Evaluation/spinA", scale_spinA->value());
    settings.setValue("Evaluation/spinB", scale_spinB->value());

    settings.setValue("Evaluation/comboA", Acolor_combobox->currentIndex());
    settings.setValue("Evaluation/comboB", Bcolor_combobox->currentIndex());

    settings.setValue("Evaluation/Ra", int(redAselected));
    settings.setValue("Evaluation/Ga", int(greenAselected));
    settings.setValue("Evaluation/Ba", int(blueAselected));
    settings.setValue("Evaluation/Rb", int(redBselected));
    settings.setValue("Evaluation/Gb", int(greenBselected));
    settings.setValue("Evaluation/Bb", int(blueBselected));


    settings.setValue("language", listWidget->currentRow());


    event->accept();
}

void ImageViewer::openFilenamePhi()
{
    fileName_phi = QFileDialog::getOpenFileName(this,
                                            tr("Open File"),
                                            last_directory_used,
                                            tr("Image Files (%1)").arg(nameFilters.join(" ")));

    open_phi();
}

void ImageViewer::open_phi()
{
    if( img1 != NULL )
    {
        if( !fileName_phi.isEmpty() )
        {
            QFileInfo fi(fileName_phi);
            last_directory_used = fi.absolutePath();

            QImage img_phi1 = QImage(fileName_phi);

            if( img_phi1.isNull() )
            {
                QMessageBox::information(this, tr("Opening error - Ofeli"),
                                         tr("Cannot load %1.").arg(fileName_phi));
                return;
            }

            img_phi1 = img_phi1.scaled(img_width, img_height, Qt::IgnoreAspectRatio, Qt::FastTransformation);

            int histogram[256];
            for( int I = 0; I < 256; I++ )
            {
                histogram[I] = 0;
            }

            if( img_phi1.format() == 3 )
            {

                int x, y; // position

                for( int offset = 0; offset < img_size; offset++ )
                {
                    // offset = x+y*img_width <==>
                    y = offset/img_width;
                    x = offset-y*img_width;

                    histogram[ *( img_phi1.scanLine(y)+x ) ]++;
                }

                const unsigned char threshold = otsu_method(histogram,img_size);

                for( int offset = 0; offset < img_size; offset++ )
                {
                    // offset = x+y*img_width <==>
                    y = offset/img_width;
                    x = offset-y*img_width;

                    if( *( img_phi1.scanLine(y)+x ) <= threshold )
                    {
                        phi_init1[offset] = 1;
                        phi_init2[offset] = 1;
                    }
                    else
                    {
                        phi_init1[offset] = -1;
                        phi_init2[offset] = -1;
                    }
                }
            }
            else
            {
                QRgb pix;

                int x, y; // position

                for( int offset = 0; offset < img_size; offset++ )
                {
                    // offset = x+y*img_width <==>
                    y = offset/img_width;
                    x = offset-y*img_width;

                    pix = img_phi1.pixel(x,y);

                    histogram[ (unsigned char) ( 0.2989*(double)(qRed(pix)) + 0.5870*(double)(qGreen(pix)) + 0.1140*(double)(qBlue(pix)) ) ]++;
                }

                const unsigned char threshold = otsu_method(histogram,img_size);

                for( int offset = 0; offset < img_size; offset++ )
                {
                    // offset = x+y*img_width <==>
                    y = offset/img_width;
                    x = offset-y*img_width;

                    pix = img_phi1.pixel(x,y);

                    if( (unsigned char) ( 0.2989*(double)(qRed(pix)) + 0.5870*(double)(qGreen(pix)) + 0.1140*(double)(qBlue(pix)) ) <= threshold )
                    {
                        phi_init1[offset] = 1;
                        phi_init2[offset] = 1;
                    }
                    else
                    {
                        phi_init1[offset] = -1;
                        phi_init2[offset] = -1;
                    }
                }
            }

            clean_boundaries(phi_init1,phi_init1_clean);
            clean_boundaries(phi_init2,phi_init2_clean);
        }

        tab_visu(onglets->currentIndex());
    }
}

void ImageViewer::save_phi()
{
    if( phi_init2_clean != NULL )
    {
        QImage img_phi_save = QImage(img_width,img_height, QImage::Format_Indexed8);
        QVector<QRgb> table(256);
        for( int I = 0; I < 256; I++ )
        {
            table[I] = qRgb(I,I,I);
        }
        img_phi_save.setColorTable(table);

        int x, y; // position

        for( int offset = 0; offset < img_size; offset++ )
        {
            // offset = x+y*img_width <==>
            y = offset/img_width;
            x = offset-y*img_width;

            if( phi_init2_clean[offset] > 0 )
            {
                *( img_phi_save.scanLine(y)+x ) = 0;
            }
            else
            {
                *( img_phi_save.scanLine(y)+x ) = 255;
            }
        }

        QString fileName_save = QFileDialog::getSaveFileName(this,
                                                             tr("Save ϕ(t=0)"),
                                                             last_directory_used + QString(tr("/initial_phi")),
                                                             "BMP (*.bmp);;JPG (*.jpg);;PNG (*.png);;PPM (*.ppm);;TIFF (*.tiff);;XBM (*.xbm);;XPM (*.xpm)");
        // sauvegarde
        img_phi_save.save(fileName_save);
    }
}

void ImageViewer::phiInit2imgPhi()
{
    if( phi_init1_clean != NULL )
    {
        img_phi = QImage(img_width,img_height, QImage::Format_Indexed8);
        QVector<QRgb> table(256);
        for( int I = 0; I < 256; I++ )
        {
            table[I] = qRgb(I,I,I);
        }
        img_phi.setColorTable(table);

        int x, y; // position

        for( int offset = 0; offset < img_size; offset++ )
        {
            // offset = x+y*img_width <==>
            y = offset/img_width;
            x = offset-y*img_width;

            if( phi_init1_clean[offset] > 0 )
            {
                *( img_phi.scanLine(y)+x ) = 0;
            }
            else
            {
                *( img_phi.scanLine(y)+x ) = 255;
            }
        }
    }
}

void ImageViewer::imgPhi2phiInit()
{
    if( phi_init1 != NULL && phi_init2 != NULL && !img_phi.isNull() )
    {
        if( img_phi.format() == 3 )
        {
            int x, y; // position

            for( int offset = 0; offset < img_size; offset++ )
            {
                // offset = x+y*img_width <==>
                y = offset/img_width;
                x = offset-y*img_width;

                if( *( img_phi.scanLine(y)+x ) < 128 )
                {
                    phi_init1[offset] = 1;
                    phi_init2[offset] = 1;
                }
                else
                {
                    phi_init1[offset] = -1;
                    phi_init2[offset] = -1;
                }
            }
        }
        else
        {
            QRgb pix;

            int x, y; // position

            for( int offset = 0; offset < img_size; offset++ )
            {
                // offset = x+y*img_width <==>
                y = offset/img_width;
                x = offset-y*img_width;

                pix = img_phi.pixel(x,y);

                if( (0.2989*(double)(qRed(pix)) + 0.5870*(double)(qGreen(pix)) + 0.1140*(double)(qBlue(pix)) ) < 128.0 )
                {
                    phi_init1[offset] = 1;
                    phi_init2[offset] = 1;
                }
                else
                {
                    phi_init1[offset] = -1;
                    phi_init2[offset] = -1;
                }
            }
        }

        clean_boundaries(phi_init1,phi_init1_clean);
        clean_boundaries(phi_init2,phi_init2_clean);
    }
}

void ImageViewer::adjustVerticalScroll(int min, int max)
{
    if( img_height != 0 )
    {
        scrollArea->verticalScrollBar()->setValue( (max-min)*positionY/img_height );
    }
}

void ImageViewer::adjustHorizontalScroll(int min, int max)
{
    if( img_width != 0 )
    {
        scrollArea->horizontalScrollBar()->setValue( (max-min)*positionX/img_width );
    }
}

void ImageViewer::adjustVerticalScroll_settings(int min, int max)
{
    if( img_height != 0 )
    {
        scrollArea_settings->verticalScrollBar()->setValue( (max-min)*positionY/img_height );
    }
}

void ImageViewer::adjustHorizontalScroll_settings(int min, int max)
{
    if( img_width != 0 )
    {
        scrollArea_settings->horizontalScrollBar()->setValue( (max-min)*positionX/img_width );
    }
}

void ImageViewer::adjustVerticalScrollA(int min, int max)
{
    if( imgA_height != 0 )
    {
        scrollAreaA->verticalScrollBar()->setValue( (max-min)*positionYa/imgA_height );
    }
}

void ImageViewer::adjustHorizontalScrollA(int min, int max)
{
    if( imgA_width != 0 )
    {
        scrollAreaA->horizontalScrollBar()->setValue( (max-min)*positionXa/imgA_width );
    }
}

void ImageViewer::adjustVerticalScrollB (int min, int max)
{
    if( imgB_height != 0 )
    {
        scrollAreaB->verticalScrollBar()->setValue( (max-min)*positionYb/imgB_height );
    }
}

void ImageViewer::adjustHorizontalScrollB(int min, int max)
{
    if( imgB_width != 0 )
    {
        scrollAreaB->horizontalScrollBar()->setValue( (max-min)*positionXb/imgB_width );
    }
}

void ImageViewer::language ()
{
    language_window->show();
}

void ImageViewer::doc()
{
    bool result = QDesktopServices::openUrl( QUrl("http://ofeli.googlecode.com/svn/doc/index.html", QUrl::TolerantMode) );
}

void ImageViewer::setCurrentFile(const QString &fileName1)
{
    curFile = fileName1;
    //setWindowFilePath(curFile);

    QSettings settings("Bessy", "Ofeli");
    QStringList files = settings.value("name/recentFileList").toStringList();
    files.removeAll(fileName1);
    files.prepend(fileName1);
    while( files.size() > MaxRecentFiles )
    {
        files.removeLast();
    }

    settings.setValue("name/recentFileList", files);

    foreach( QWidget *widget, QApplication::topLevelWidgets() )
    {
        ImageViewer *imageViewer = qobject_cast<ImageViewer *>(widget);
        if( imageViewer )
        {
            imageViewer->updateRecentFileActions();
        }
    }
}

void ImageViewer::updateRecentFileActions()
{
    QSettings settings("Bessy", "Ofeli");
    QStringList files = settings.value("name/recentFileList").toStringList();

    int numRecentFiles = qMin(files.size(), (int)MaxRecentFiles);

    for( int i = 0; i < numRecentFiles; ++i )
    {
        QString text = tr("&%1   %2").arg(i + 1).arg( strippedName(files[i]) );
        recentFileActs[i]->setText(text);
        recentFileActs[i]->setData(files[i]);
        recentFileActs[i]->setVisible(true);
        recentFileActs[i]->setStatusTip(files[i]);
    }

    for( int j = numRecentFiles; j < MaxRecentFiles; ++j )
    {
        recentFileActs[j]->setVisible(false);
    }

    separatorAct->setVisible(numRecentFiles > 0);

    if( files.isEmpty() )
    {
        deleteAct->setVisible(false);
    }
    else
    {
        deleteAct->setVisible(true);
    }
}

QString ImageViewer::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

void ImageViewer::openRecentFile()
{
    QAction* action = qobject_cast<QAction*>( sender() );
    if( action )
    {
        fileName = action->data().toString();
    }

    open();
}

void ImageViewer::deleteList()
{
    QStringList files;
    files.clear();

    QSettings settings("Bessy", "Ofeli");
    settings.setValue("name/recentFileList", files);

    updateRecentFileActions();
}

unsigned char ImageViewer::otsu_method(const int histogram[], int img_size)
{
    int sum = 0;
    for( int I = 0; I < 256; I++ )
    {
        sum += I*histogram[I];
    }

    int weight1, weight2, sum1, threshold;
    double mean1, mean2, var_t, var_max;

    threshold = 127; // value returned in the case of an totally homogeneous image

    weight1 = 0;
    sum1 = 0;
    var_max = -1.0;

    // 256 values ==> 255 thresholds t evaluated
    // class1 <= t and class2 > t
    for( int t = 0; t < 255; t++ )
    {
        weight1 += histogram[t];
        if( weight1 == 0 )
        {
            continue;
        }

        weight2 = img_size-weight1;
        if( weight2 == 0 )
        {
            break;
        }

        sum1 += t*histogram[t];

        mean1 = sum1/weight1;
        mean2 = (sum-sum1)/weight2; // sum2 = sum-sum1

        var_t = double(weight1)*double(weight2)*(mean1-mean2)*(mean1-mean2);

        if( var_t > var_max )
        {
            var_max = var_t;
            threshold = t;
        }
    }

    return (unsigned char)(threshold);
}

}
