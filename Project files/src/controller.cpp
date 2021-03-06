#include <iostream>
#include <cstddef>
#include <fstream>
#include <SFML/Graphics.hpp>
//#include "FileIO.h"
#include"id.h"
#include "controller.h"
#include "error.h"
#include "drawer.h"
#include "matrix.h"

Controller::Controller()
{
};

ID Controller::addPrimitive(PrimitiveType type, Storage<double> params, ID* id)
{
	switch (type)
	{
	case P_Point:
		// Проверяем параметры
		if (params.size() == 2)
		{
			// Добавляем точку
			if (id)
			{
				m_points.add(*id, Point(params[0], params[1]));
				return *id;
			}
			else
			{
				ID n_id;
				m_points.add(n_id, Point(params[0], params[1]));
				return n_id;
			}
		}
		else
			throw BadArgument();
		break;

	case P_Segment:
		// Проверяем параметры
		if (params.size() == 4)
		{
			if (id)
			{
				Point* start = new Point(params[0], params[1]);
				Point* end = new Point(params[2], params[3]);
				m_segments.add(*id, Segment(start, end));
				return *id;
			}
			else
			{

				ID id1;
				// Добавляем точки, определяющие отрезок
				m_points.add(id1, Point(params[0], params[1]));
				Point* start = &m_points.getAssoc(id1);

				ID id2;
				m_points.add(id2, Point(params[2], params[3]));
				Point* end = &m_points.getAssoc(id2);

				// И сам отрезок
				ID id3;
				m_segments.add(id3, Segment(start, end));
				return id3;
			}
		}
		else
			throw BadArgument();
		break;

	case P_Circle:
		// Проверяем параметры
		if (params.size() == 3)
		{
			if (id)
			{
				Point* center = new Point(params[0], params[1]);
				m_circles.add(*id, Circle(center, params[2]));
				return *id;
			}
			else
			{
				// Добавляем центр окружности
				ID id1;
				m_points.add(id1, Point(params[0], params[1]));
				Point* center = &m_points.getAssoc(id1);

				// И саму окружность
				ID id2;
				m_circles.add(id2, Circle(center, params[2]));
				return id2;
			}
		}
		else
			throw BadArgument();
		break;

	default:
		// Выводим ошибку в случае отсутствия реализации для типа
		std::cerr << "No implementation found!" << std::endl;
		throw BadArgument();
	}

}

void Controller::updateView()
{
	// Проходим по всем типам примитивов и рисуем их
	MMapAVL<ID, Point>::Marker markerPoint = m_points.createMarker();
	while (markerPoint.isValid())
	{
		drawer->drawPrimitive(P_Point, markerPoint.getValue().value.getParams());
		markerPoint.next();
	}

	MMapAVL<ID, Segment>::Marker markerSegment = m_segments.createMarker();
	while (markerSegment.isValid())
	{
		drawer->drawPrimitive(P_Segment, markerSegment.getValue().value.getParams());
		markerSegment.next();
	}

	MMapAVL<ID, Circle>::Marker markerCircle = m_circles.createMarker();
	while (markerCircle.isValid())
	{
		drawer->drawPrimitive(P_Circle, markerCircle.getValue().value.getParams());
		markerCircle.next();
	}
}


void Controller::removePrimitive(const ID& id)
{
	MMapAVL<ID, Point>::Marker pointMarker = m_points.createMarker();
	while (pointMarker.isValid())
	{
		if (pointMarker.getValue().key == id)
		{
			pointMarker.remove();
			return;
		}
		pointMarker.next();
	}

	MMapAVL<ID, Segment>::Marker segmentMarker = m_segments.createMarker();
	while (segmentMarker.isValid())
	{
		if (segmentMarker.getValue().key == id)
		{
			segmentMarker.remove();
			return;
		}
		segmentMarker.next();
	}

	MMapAVL<ID, Circle>::Marker circleMarker = m_circles.createMarker();
	while (circleMarker.isValid())
	{
		if (circleMarker.getValue().key == id)
		{
			circleMarker.remove();
			return;
		}
		circleMarker.next();
	}
}

void Controller::removeRequirement(const ID& id)
{
	MMapAVL<ID, Requirement>::Marker requirementMarker = m_requirements.createMarker();
	while (requirementMarker.isValid())
	{
		if (requirementMarker.getValue().key == id)
		{
			requirementMarker.remove();
			return;
		}
		requirementMarker.next();
	}
}

Pair<PrimitiveType, Storage<double>> Controller::getPrimitiveInfo(ID& id)
{
	MMapAVL<ID, Point>::Marker markerPoint = m_points.createMarker();
	while (markerPoint.isValid())
	{
		if (markerPoint.getValue().key == id)
			return Pair<PrimitiveType, Storage<double>>(P_Point, markerPoint.getValue().value.getParams());
		markerPoint.next();
	}
	MMapAVL<ID, Segment>::Marker markerSegment = m_segments.createMarker();
	while (markerSegment.isValid())
	{
		if (markerSegment.getValue().key == id)
			return Pair<PrimitiveType, Storage<double>>(P_Segment, markerSegment.getValue().value.getParams());
		markerSegment.next();
	}
	MMapAVL<ID, Circle>::Marker markerCircle = m_circles.createMarker();
	while (markerCircle.isValid())
	{
		if (markerCircle.getValue().key == id)
			return Pair<PrimitiveType, Storage<double>>(P_Circle, markerCircle.getValue().value.getParams());
		markerCircle.next();
	}
	throw BadArgument();
}



Storage<ID> Controller::getAllPrimitiveIDs()
{
	Storage<ID> allIDs;
	MMapAVL<ID, Point>::Marker markerPoint = m_points.createMarker();
	while (markerPoint.isValid())
	{
		allIDs.add(markerPoint.getValue().key);
		markerPoint.next();
	}
	MMapAVL<ID, Segment>::Marker markerSegment = m_segments.createMarker();
	while (markerSegment.isValid())
	{
		allIDs.add(markerSegment.getValue().key);
		markerSegment.next();
	}
	MMapAVL<ID, Circle>::Marker markerCircle = m_circles.createMarker();
	while (markerCircle.isValid())
	{
		allIDs.add(markerCircle.getValue().key);
		markerCircle.next();
	}
	return allIDs;
}

/*bool Controller::tryAddRequirement(RequirementType req, const Storage<ID>& ids, double* param = nullptr)
{
	switch (req)
	{
	case R_PointOnSegment:
	{
		ID id;
		Requirement R;
		R.type = req;
		R.param = param;
		R.objects = ids;

		m_requirements.add(Pair<ID,Requirement>(id,R));

		class TotalErrCalc
		{
		private:
			Storage<IRequirement*>
		public:
			Storage<double> operator()(const Storage<double> &x);
			Matrix<double> derivative(const Storage<double> &x);
		};
	}
	}
}*/


//ID Controller::addRequirement(RequirementType, const Storage<ID>&, double* param)
//{
//    backupState();
//    if (tryAddRequirement())
//    {
//        m_requirements.add(Requirement());
//    }
//    else
//    {
//        restoreState();
//    }
//}

void Controller::readPrimitive(const std::string& filename)
{
	FileIO::read(filename, *this);
}


void Controller::writePrimitive(const std::string& filename)
{

	MMapAVL<ID, Point>::Marker pointMarker = m_points.createMarker();
	while (pointMarker.isValid())
	{
		FileIO::write(filename, P_Point, Pair<ID, Storage <double>>(pointMarker.getValue().key, pointMarker.getValue().value.getParams()));
		pointMarker.next();
	}

	MMapAVL<ID, Segment>::Marker segmentMarker = m_segments.createMarker();
	while (segmentMarker.isValid())
	{
		FileIO::write(filename, P_Segment, Pair<ID, Storage<double>>(segmentMarker.getValue().key, segmentMarker.getValue().value.getParams()));
		segmentMarker.next();
	}

	MMapAVL<ID, Circle>::Marker circleMarker = m_circles.createMarker();
	while (circleMarker.isValid())
	{
		FileIO::write(filename, P_Circle, Pair<ID, Storage<double>>(circleMarker.getValue().key, circleMarker.getValue().value.getParams()));
		circleMarker.next();
	}
};


void Controller::writeRequirement(const std::string& filename)
{
};
void Controller::readRequirement(const std::string& filename)
{
};
