#ifndef printerptrvector_h
#define printerptrvector_h

class Printer;

/**
 * \class PrinterPtrVector
 * \brief This class implements a dynamic vector of Printer values
 */
class PrinterPtrVector {
public:
  /**
   * \brief This is the default PrinterPtrVector constructor
   */
  PrinterPtrVector() { size = 0; v = 0; };
  /**
   * \brief This is the PrinterPtrVector constructor for a specified size
   * \param sz this is the size of the vector to be created
   * \note The elements of the vector will all be created, and set to zero
   */
  PrinterPtrVector(int sz);
  /**
   * \brief This is the PrinterPtrVector constructor for a specified size with an initial value
   * \param sz this is the size of the vector to be created
   * \param initial this is the initial value for all the entries of the vector
   */
  PrinterPtrVector(int sz, Printer* initial);
  /**
   * \brief This is the PrinterPtrVector constructor that create a copy of an existing PrinterPtrVector
   * \param initial this is the PrinterPtrVector to copy
   */
  PrinterPtrVector(const PrinterPtrVector& initial);
  /**
   * \brief This is the PrinterPtrVector destructor
   * \note This will free all the memory allocated to all the elements of the vector
   */
  ~PrinterPtrVector();
  /**
   * \brief This will add new entries to the vector
   * \param add this is the number of new entries to the vector
   * \param value this is the value that will be entered for the new entries
   */
  void resize(int add, Printer* value);
  /**
   * \brief This will add new empty entries to the vector
   * \param add this is the number of new entries to the vector
   * \note The new elements of the vector will be created, and set to zero
   */
  void resize(int add);
  /**
   * \brief This will delete an entry from the vector
   * \param pos this is the element of the vector to be deleted
   * \note This will free the memory allocated to the deleted element of the vector
   */
  void Delete(int pos);
  /**
   * \brief This will return the size of the vector
   * \return the size of the vector
   */
  int Size() const { return size; };
  /**
   * \brief This will return the value of an element of the vector
   * \param pos this is the element of the vector to be returned
   * \return the value of the specified element
   */
  Printer*& operator [] (int pos);
  /**
   * \brief This will return the value of an element of the vector
   * \param pos this is the element of the vector to be returned
   * \return the value of the specified element
   */
  Printer* const& operator [] (int pos) const;
protected:
  /**
   * \brief This is the vector of Printer values
   */
  Printer** v;
  /**
   * \brief This is size of the vector
   */
  int size;
};

#ifdef GADGET_INLINE
#include "printerptrvector.icc"
#endif

#endif
