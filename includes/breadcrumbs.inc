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
        $exclusion   = flat_breadcrumbs_exclusion_list();
        $root        = variable_get('islandora_repository_pid', 'islandora:root');
        $breadcrumbs = [];

        $ignore = true;
        foreach ($data['tree'] as $item) {

            if (true === $ignore) {

                // ignoring tree until root
                if ($item['pid'] === $root) {
                    $ignore = false;
                }

                continue;
            }

            if (in_array($item['pid'], $exclusion)) {
                continue;
            }

            $breadcrumbs[] = l($item['label'], 'islandora/object/' . $item['pid']);
        }

        if (count($breadcrumbs) > 0) {

            // breadcrumbs found, adding default breadcrumbs
            $breadcrumbs = array_merge(flat_breadcrumbs_default_breadcrumbs(), $breadcrumbs);

            // and removing last breadcrumb, because it is active
            array_pop($breadcrumbs);
        }

        return $breadcrumbs;
    }
}
