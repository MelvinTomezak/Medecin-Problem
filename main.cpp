#include <iostream>
#include <thread>
#include <vector>
#include <semaphore.h>
#include <chrono>

using namespace std;

int n = 5;
int patients = 0;
int examinedPatients = 0;

sem_t mutex_sem;
sem_t patient_sem;
sem_t medecin_sem;
sem_t patientDone_sem;
sem_t medecinDone_sem;
sem_t allDone_sem;
sem_t sleepingMedecin_sem;

void semaphores_init() {
    sem_init(&mutex_sem, 0, 1);
    sem_init(&patient_sem, 0, 0);
    sem_init(&medecin_sem, 0, 0);
    sem_init(&patientDone_sem, 0, 0);
    sem_init(&medecinDone_sem, 0, 0);
    sem_init(&allDone_sem, 0, 0);
    sem_init(&sleepingMedecin_sem, 0, 0);
}

void semaphores_destroy() {
    sem_destroy(&mutex_sem);
    sem_destroy(&patient_sem);
    sem_destroy(&medecin_sem);
    sem_destroy(&patientDone_sem);
    sem_destroy(&medecinDone_sem);
    sem_destroy(&allDone_sem);
}

void consultation(int patientId) {
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


void examiner() {
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

    thread medecinThread(examiner);

    vector<thread> patientThreads;
    int patientId = 1;

    while (patientId <= 10) {
        patientThreads.push_back(thread(consultation, patientId));
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
