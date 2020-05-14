<?php
namespace TLA\FLAT\Breadcrumbs\Exception;

use TLA\FLAT\Breadcrumbs\Exception\BaseException;

/**
 * Exception thrown if breadcrumbs have already been fetched. This is used
 * to prevent setting the breadcrumbs multiple times in a single request.
 *
 * @author  Ibrahim Abdullah <ibrahim.abdullah@mpi.nl>
 * @package TLA FLAT Breadcrumbs
 */
class AlreadyFetchedException extends BaseException
{
}
