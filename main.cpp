#include <iostream>
#include <thread>
#include <vector>
#include <semaphore.h>
#include <chrono>

using namespace std;

int n = 5; // nombre de siége disponible
int patients = 0; // on initie le nombre de patient a 0
int examinedPatients = 0; //on initie le nombre de patient examiner a 0

// on déclare nos différents sémaphore
sem_t mutex_sem;
sem_t patient_sem;
sem_t medecin_sem;
sem_t patientDone_sem;
sem_t medecinDone_sem;
sem_t allDone_sem;
sem_t sleepingMedecin_sem;

// La fonction semaphores_init nous permet d'initier nos différents semaphores.
// Si la valeur = 0 alors on partage la valeur uniquement entre les threads d'un même processus
// Si la valeur = 1 c'est qu'on a un mutex
// Un mutex = objet utilisé pour garantir qu'un thread puisse éxecuter une partie du code.
void semaphores_init() {
    sem_init(&mutex_sem, 0, 1); // synchronisation de threads, 1 indique qu'il est disponible
    sem_init(&patient_sem, 0, 0);
    sem_init(&medecin_sem, 0, 0);
    sem_init(&patientDone_sem, 0, 0);
    sem_init(&medecinDone_sem, 0, 0);
    sem_init(&allDone_sem, 0, 0);
    sem_init(&sleepingMedecin_sem, 0, 0);
}
// La fonction semaphores_destroy, nous permet de détruire les sémaphores qu'on a utilisé précedement.
// Cela permet de libérer les ressources associé au sémaphore précedement initié.
void semaphores_destroy() {
    sem_destroy(&mutex_sem);
    sem_destroy(&patient_sem);
    sem_destroy(&medecin_sem);
    sem_destroy(&patientDone_sem);
    sem_destroy(&medecinDone_sem);
    sem_destroy(&allDone_sem);
}

// La fonction attente nous permets de faire en sorte qu'un patient parte s'il y a pas de chaise disponible
// Ou de le faire entrer si une chaise est disponible
void attente(int patientId) {
    sem_wait(&mutex_sem);

    if (patients == n) {
        sem_post(&mutex_sem);
        cout << "Le patient " << patientId << " s'en va car il n'y a pas de chaise disponible." << endl;
        return;
    }

    patients++;
    cout << "Le patient " << patientId << " entre car il y a une chaise disponible : " << (n - patients) << endl;

    sem_post(&patient_sem);
    sem_post(&mutex_sem);
}

// La fonction consultation permet au médecin de prendre un patient en consultation pendant une durée détérminé.
// Et de limiter le nombre de patient souhaiter pour simuler une journée de travail.
void consultation() {
    while (true) {
        sem_wait(&patient_sem);

        cout << "Le médecin est prêt pour la prochaine consultation." << endl;
        cout << "Un patient est en cours de consultation." << endl;

        this_thread::sleep_for(chrono::milliseconds(500));  // Simule l'examen
        cout << "Le médecin a terminer la consultation, tout s'est bien passé." << endl;

        sem_wait(&mutex_sem);
        patients--;
        examinedPatients++;
        cout << "Le patient quitte le cabinet, il y a maintenant " << (n - patients) << " chaises disponibles : " << endl;
        sem_post(&mutex_sem);

        if (examinedPatients == 10) {
            break;
        }
    }
    cout << "Le cabinet ferme ces portes, c'est une fin de journée." << endl;
}

int main() {
    semaphores_init();

    thread medecinThread(consultation);

    vector<thread> patientThreads;
    int patientId = 1;

    while (patientId <= 10) {
        patientThreads.push_back(thread(attente, patientId));
        patientId++;
        this_thread::sleep_for(chrono::milliseconds(400));
    }

    medecinThread.join();
    for (auto &t : patientThreads) {
        t.join();
    }

    semaphores_destroy();
    return 0;
}
