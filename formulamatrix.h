#ifndef formulamatrix_h
#define formulamatrix_h

#include "intvector.h"
#include "formulavector.h"

/**
 * \class FormulaMatrix
 * \brief This class implements a dynamic vector of FormulaVector values
 */
class FormulaMatrix {
public:
  /**
   * \brief This is the default FormulaMatrix constructor
   */
  FormulaMatrix() { nrow = 0; v = 0; };
  /**
   * \brief This is the FormulaMatrix constructor for a specified size
   * \param nrow this is the size of the vector to be created
   * \param ncol this is the length of each row to be created (ie. the size of the FormulaVector to be created for each row)
   * \note The elements of the vector will all be created, and set to zero
   */
  FormulaMatrix(int nrow, int ncol);
  /**
   * \brief This is the FormulaMatrix constructor for a specified size
   * \param nrow this is the size of the vector to be created
   * \param ncol this is the length of each row to be created (ie. the size of the FormulaVector to be created for each row)
   * \param initial this is the initial value for all the entries of the vector
   */
  FormulaMatrix(int nrow, int ncol, Formula initial);
  /**
   * \brief This is the FormulaMatrix constructor for a specified size
   * \param nrow this is the size of the vector to be created
   * \param ncol this is the length of the rows to be created (ie. the size of the FormulaVector to be created for each row)
   * \note The elements of the vector will all be created, and set to zero
   */
  FormulaMatrix(int nrow, const IntVector& ncol);
  /**
   * \brief This is the FormulaMatrix destructor
   * \note This will free all the memory allocated to all the elements of the vector
   */
  ~FormulaMatrix();
  FormulaMatrix& operator = (const FormulaMatrix& formulaM);
  /**
   * \brief This will return the number of columns in row i of the vector
   * \param i this is the row of the vector to have the number of columns counted
   * \return the number of columns in row i of the vector
   * \note This is the number of entries in the FormulaVector that is entry i of the FormulaMatrix
   */
  int Ncol(int i = 0) const { return v[i]->Size(); };
  /**
   * \brief This will return the number of rows of the vector
   * \return the number of rows of the vector
   */
  int Nrow() const { return nrow; };
  /**
   * \brief This will return the value of an element of the vector
   * \param pos this is the element of the vector to be returned
   * \return the value of the specified element
   */
  FormulaVector& operator [] (int pos);
  /**
   * \brief This will return the value of an element of the vector
   * \param pos this is the element of the vector to be returned
   * \return the value of the specified element
   */
  const FormulaVector& operator [] (int pos) const;
  /**
   * \brief This will add new empty entries to the vector
   * \param add this is the number of new entries to the vector
   * \param length this is the number of entries to the FormulaVector that is created
   * \note The new elements of the vector will be created, and set to zero
   */
  void AddRows(int add, int length);
  /**
   * \brief This will add new entries to the vector
   * \param add this is the number of new entries to the vector
   * \param length this is the number of entries to the FormulaVector that is created
   * \param formula this is the value that will be entered for the new entries
   */
  void AddRows(int add, int length, Formula formula);
  void Inform(Keeper* keeper);
  friend CommentStream& operator >> (CommentStream& infile, FormulaMatrix& Fmatrix);
protected:
  /**
   * \brief This is number of rows of the vector
   */
  int nrow;
  /**
   * \brief This is the vector of FormulaVector values
   */
  FormulaVector** v;
};

#ifdef GADGET_INLINE
#include "formulamatrix.icc"
#endif

#endif
