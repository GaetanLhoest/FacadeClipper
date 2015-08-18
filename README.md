# FacadeClipper

At this moment, a study is performed in order to present a user-oriented protocol that can be used to characterize chromatic attributes of an urban fragment. No tools performing the clipping to extract the chromatic attributes were available, and the extraction had to be performed manually.  Finding a way to speed up the process was then a serious concern.

This project aim to provide a functional solution to speed up the process of extraction of the chromatic attributes and decrease the manual work on the facade photographs. In other words, obtaining the best extraction results with the less amount of work and in the shorter computation time as possible.

Our solution is a model free approach.

It can be decomposed in 3 steps:

* The first step is the collect of data.  This collect of data ask the user to provide useful information on the main color area.

* The second step is the construction of the primary result.  The primary result is a basic approximation of the area we want to extract.  We obtain the primary result with same
texture searching, color analysis and floodfill algorithm.

* The third step is the enhancement of the primary result.  The enhancement is obtained by  combining  the  results  of  diㄦent  methods  using  the  primary  result.   We  combine the results of the power watershed algorithm and what we called the Canny fooding algorithm.


This project uses the OpenCV library, the Qt library and the Power Watershed algorithm from Camille Couprie, Leo Grady, Laurent Najman and Hugues Talbot.
