<?php
namespace TLA\FLAT\Breadcrumbs\Backend;

use TLA\FLAT\Breadcrumbs\Exception\NotFoundException;
use TLA\FLAT\Breadcrumbs\Exception\InvalidQueryException;
use TLA\FLAT\Breadcrumbs\Exception\AlreadyFetchedException;
use Apache_Solr_Service;
use Exception;

/**
 * Solr backend, using singleton pattern because islandora uses "islandora_drupal_title"
 * which gets called multiple times per request.
 *
 * @author  Ibrahim Abdullah <ibrahim.abdullah@mpi.nl>
 * @package TLA FLAT Breadcrumbs
 */
class SolrBackend
{
    /**
     * @var self
     */
    private static $instance = null;

    /**
     * @var Apache_Solr_Service
     */
    private $solr;

    /**
     * @var array
     */
    private $data;

    /**
     * Marking constructor as private to enforce singleton
     */
    private function __construct() {}

    /**
     * Creating singleton
     *
     * @return self
     */
    public static function create(): self
    {
        if (null === static::$instance) {
            static::$instance = new self();
        }

        return static::$instance;
    }

    /**
     * Fetching data for entire tree of object id
     *
     * @param array $pids
     *
     * @return array
     * @throws NotFoundException
     * @throws InvalidQueryException
     * @throws AlreadyFetchedException
     */
    public function fetch(string $pid)
    {
        if (null !== $this->data) {

            // data from solr was already fetched
            throw new AlreadyFetchedException('Breadcrumbs data has already being fetched in this request');
        }

        // first getting current object data
        $object = $this->getData([$pid])['response']['docs'][0];

        // initializing data
        $this->data = [

            'object'    => $this->normalizeDocument($object),
            'parents'   => [],
            'ancestors' => [],
            'tree'      => [],
        ];
        if (count($this->data['object']['ancestors']) > 0) {

            // ancestors found, fetching data
            $pids = array_merge($this->data['object']['ancestors'], $this->data['object']['parents']);
            $data = $this->getData($pids);

            // normalizing ancestors and parents
            foreach ($data['response']['docs'] as $document) {

                $normalized = $this->normalizeDocument($document);

                if (in_array($normalized['pid'], $this->data['object']['ancestors'])) {
                    $this->data['ancestors'][] = $normalized;
                }

                if (in_array($normalized['pid'], $this->data['object']['parents'])) {
                    $this->data['parents'][] = $normalized;
                }
            }

            // assigning ranks for ancestors to preserve order of ancestors
            $rank  = 0;
            $ranks = [];

            foreach ($this->data['object']['ancestors'] as $pid)  {

                $rank       += 1;
                $ranks[$pid] = $rank;
            }

            foreach ($this->data['ancestors'] as $key => $ancestor) {
                $this->data['ancestors'][$key]['rank'] = $ranks[$ancestor['pid']] ?? 0;
            }

            // sorting tree
            $this->data['tree'] = $this->data['ancestors'];

            usort($this->data['tree'], function($a, $b) {
                return $b['rank'] <=> $a['rank'];
            });
        }

        // setting current object to active
        $current           = $this->data['object'];
        $current['active'] = true;

        // adding parents if available
        foreach ($this->data['parents'] as $parent) {
            $this->data['tree'][] = $parent;
        }

        // adding current object to tree
        $this->data['tree'][] = $current;

        // and finally returning data
        return $this->data;
    }

    /**
     * Getting solr documents
     *
     * @param string $pid
     *
     * @return array
     * @throws NotFoundException
     * @throws InvalidQueryException
     */
    public function getData(array $pids): array
    {
        $params = $this->params($pids);
        $data   = $this->query($params);

        if (false === $data || $data['response']['numFound'] <= 0) {
            throw new NotFoundException(sprintf('Could not get solr data for pids %s', implode(', ', $pids)));
        }

        return $data;
    }

    /**
     * Normalizing solr document
     *
     * @param array $document
     *
     * @return array
     */
    private function normalizeDocument(array $document): array
    {
        return [

            'pid'       => $document['PID'],
            'label'     => $document['fgs_label_s'],
            'roles'     => $document['policy_datastream_roles_ms'] ?? [],
            'users'     => $document['policy_datastream_users_ms'] ?? [],
            'parents'   => $document['RELS_EXT_isConstituentOf_uri_ms'] ?? [],
            'children'  => $document['compound_children_ms'] ?? [],
            'ancestors' => $document['ancestors_ms'] ?? [],
            'rank'      => 0,
            'active'    => false,
        ];
    }

    /**
     * Connect to apache solr
     *
     * @return Apache_Solr_Service
     */
    private function solr()
    {
        if (null !== $this->solr) {
            return $this->solr;
        }

        $path_parts = parse_url(variable_get('islandora_solr_url', 'localhost:8080/solr'));

        $this->solr = new Apache_Solr_Service($path_parts['host'], $path_parts['port'], $path_parts['path'] . '/');
        $this->solr->setCreateDocuments(0);

        return $this->solr;
    }

    /**
     * Running solr query
     *
     * @param array $params
     *
     * @return array
     * @throws InvalidQueryException
     */
    private function query(array $params): array
    {
        try {
            $results = $this->solr()->search('*:*', 0, 1000, $params);
        } catch (Exception $e) {
            throw new InvalidQueryException($e->getMessage());
        }

        return json_decode($results->getRawResponse(), true);
    }

    /**
     * Preparing params
     *
     * @param array $pids
     *
     * @return array
     */
    private function params(array $pids): array
    {
        $params = [

            'sort' => 'fgs_label_ss asc',
            'fl'   => 'PID, fgs_label_s, policy_datastream_roles_ms, policy_datastream_users_ms, ancestors_ms, RELS_EXT_isConstituentOf_uri_ms, compound_children_ms',
            'fq'   => [
                format_string('({!terms f=PID}!pids)', ['!pids' => implode(',', $pids)]),
            ],
        ];

        return $params;
    }
}
