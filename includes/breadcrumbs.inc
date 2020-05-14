<?php
namespace TLA\FLAT\Breadcrumbs;

use TLA\FLAT\Breadcrumbs\Backend\SolrBackend;
use TLA\FLAT\Breadcrumbs\Exception\NotFoundException;
use TLA\FLAT\Breadcrumbs\Exception\InvalidQueryException;

/**
 * Generating breadcrumbs for current object
 *
 * @author  Ibrahim Abdullah <ibrahim.abdullah@mpi.nl>
 * @package TLA FLAT Breadcrumbs
 */
class Breadcrumbs
{
    /**
     * Generating breadcrumbs for pid
     *
     * @param string $pid
     *
     * @return array|false
     * @throws NotFoundException
     * @throws InvalidQueryException
     */
    public static function get(string $pid): array
    {
        try {

            return static::breadcrumbs($pid);

        } catch (NotFoundException $e) {

            // no breadcrumbs so page can still render (without breadcrumbs)
            return [];

        } catch (InvalidQueryException $e) {

            // no breadcrumbs so page can still render (without breadcrumbs)
            return [];
        }
    }

    /**
     * Getting breadcrumbs
     *
     * @param string $pid
     *
     * @return array
     * @throws NotFoundException
     * @throws InvalidQueryException
     */
    private static function breadcrumbs(string $pid): array
    {
        $backend     = SolrBackend::create();
        $data        = $backend->fetch($pid);
        $breadcrumbs = [];
        $root        = flat_breadcrumbs_db_get_root();

        if (count($data['tree']) > 0) {

            // if current object has ancestors, prepend root
            $breadcrumbs[] = $root['link'];
        }

        foreach ($data['tree'] as $item) {

            if (false === $item['active']) {

                // only generating breadcrumb link for inactive objects
                // islandora adds current active object to breadcrumb automatically
                $breadcrumbs[] = l($item['label'], 'islandora/object/' . $item['pid']);
            }
        }

        return $breadcrumbs;
    }
}
